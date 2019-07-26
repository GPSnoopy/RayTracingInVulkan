#include "Application.hpp"
#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "RayTracingPipeline.hpp"
#include "ShaderBindingTable.hpp"
#include "TopLevelAccelerationStructure.hpp"
#include "Assets/Model.hpp"
#include "Assets/Scene.hpp"
#include "Utilities/Glm.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageMemoryBarrier.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/SingleTimeCommands.hpp"
#include "Vulkan/SwapChain.hpp"
#include <chrono>
#include <iostream>
#include <numeric>

namespace Vulkan::RayTracing {

namespace
{
	AccelerationStructure::MemoryRequirements GetTotalRequirements(const std::vector<AccelerationStructure::MemoryRequirements>& requirements)
	{
		AccelerationStructure::MemoryRequirements total{};

		for (const auto& req : requirements)
		{
			total.Result.size += req.Result.size;
			total.Build.size += req.Build.size;
			total.Update.size += req.Update.size;
		}

		return total;
	}
}

Application::Application(const WindowConfig& windowConfig, const bool vsync, const bool enableValidationLayers) :
	Vulkan::Application(windowConfig, vsync, enableValidationLayers)
{
}

Application::~Application()
{
	Application::DeleteSwapChain();
	DeleteAccelerationStructures();

	deviceProcedures_.reset();
	properties_.reset();
}

void Application::OnDeviceSet()
{
	properties_.reset(new RayTracingProperties(Device()));
	deviceProcedures_.reset(new DeviceProcedures(Device()));
}

void Application::CreateAccelerationStructures()
{
	std::cout << "Building acceleration structures..." << std::endl;
	const auto timer = std::chrono::high_resolution_clock::now();

	SingleTimeCommands::Submit(CommandPool(), [this](VkCommandBuffer commandBuffer)
	{
		CreateBottomLevelStructures(commandBuffer);
		AccelerationStructure::MemoryBarrier(commandBuffer);
		CreateTopLevelStructures(commandBuffer);
	});

	topScratchBuffer_.reset();
	topScratchBufferMemory_.reset();
	bottomScratchBuffer_.reset();
	bottomScratchBufferMemory_.reset();

	const auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();
	std::cout << "Built acceleration structures in " << elapsed << "s" << std::endl;
}

void Application::DeleteAccelerationStructures()
{
	topAs_.clear();
	instancesBuffer_.reset();
	instancesBufferMemory_.reset();
	topScratchBuffer_.reset();
	topScratchBufferMemory_.reset();
	topBuffer_.reset();
	topBufferMemory_.reset();

	bottomAs_.clear();
	bottomScratchBuffer_.reset();
	bottomScratchBufferMemory_.reset();
	bottomBuffer_.reset();
	bottomBufferMemory_.reset();
}

void Application::CreateSwapChain()
{
	Vulkan::Application::CreateSwapChain();

	CreateOutputImage();

	rayTracingPipeline_.reset(new RayTracingPipeline(*deviceProcedures_, SwapChain(), topAs_[0], *accumulationImageView_, *outputImageView_, UniformBuffers(), GetScene()));

	const std::vector<ShaderBindingTable::Entry> rayGenPrograms = { {rayTracingPipeline_->RayGenShaderIndex(), {}} };
	const std::vector<ShaderBindingTable::Entry> missPrograms = { {rayTracingPipeline_->MissShaderIndex(), {}} };
	const std::vector<ShaderBindingTable::Entry> hitGroups = { {rayTracingPipeline_->TriangleHitGroupIndex(), {}}, {rayTracingPipeline_->ProceduralHitGroupIndex(), {}} };

	shaderBindingTable_.reset(new ShaderBindingTable(*deviceProcedures_, *rayTracingPipeline_, *properties_, rayGenPrograms, missPrograms, hitGroups));
}

void Application::DeleteSwapChain()
{
	shaderBindingTable_.reset();
	rayTracingPipeline_.reset();
	outputImageView_.reset();
	outputImage_.reset();
	outputImageMemory_.reset();
	accumulationImageView_.reset();
	accumulationImage_.reset();
	accumulationImageMemory_.reset();

	Vulkan::Application::DeleteSwapChain();
}

void Application::Render(VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
	const auto extent = SwapChain().Extent();

	VkDescriptorSet descriptorSets[] = { rayTracingPipeline_->DescriptorSet(imageIndex) };

	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	ImageMemoryBarrier::Insert(commandBuffer, accumulationImage_->Handle(), subresourceRange, 0, 
		VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	ImageMemoryBarrier::Insert(commandBuffer, outputImage_->Handle(), subresourceRange, 0, 
		VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, rayTracingPipeline_->Handle());
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, rayTracingPipeline_->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);

	deviceProcedures_->vkCmdTraceRaysNV(commandBuffer,
		shaderBindingTable_->Buffer().Handle(), shaderBindingTable_->RayGenOffset(),
		shaderBindingTable_->Buffer().Handle(), shaderBindingTable_->MissOffset(), shaderBindingTable_->MissEntrySize(),
		shaderBindingTable_->Buffer().Handle(), shaderBindingTable_->HitGroupOffset(), shaderBindingTable_->HitGroupEntrySize(),
		nullptr, 0, 0,
		extent.width, extent.height, 1);

	ImageMemoryBarrier::Insert(commandBuffer, outputImage_->Handle(), subresourceRange, 
		VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	ImageMemoryBarrier::Insert(commandBuffer, SwapChain().Images()[imageIndex], subresourceRange, 0,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageCopy copyRegion;
	copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.dstOffset = { 0, 0, 0 };
	copyRegion.extent = { extent.width, extent.height, 1 };

	vkCmdCopyImage(commandBuffer,
		outputImage_->Handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		SwapChain().Images()[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copyRegion);

	ImageMemoryBarrier::Insert(commandBuffer, SwapChain().Images()[imageIndex], subresourceRange, VK_ACCESS_TRANSFER_WRITE_BIT,
		0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void Application::CreateBottomLevelStructures(VkCommandBuffer commandBuffer)
{
	const auto& scene = GetScene();
	
	// Bottom level acceleration structure
	// Triangles via vertex buffers. Procedurals via AABBs.
	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;
	uint32_t aabbOffset = 0;

	std::vector<AccelerationStructure::MemoryRequirements> requirements;

	for (const auto& model : scene.Models())
	{
		const auto vertexCount = static_cast<uint32_t>(model.NumberOfVertices());
		const auto indexCount = static_cast<uint32_t>(model.NumberOfIndices());
		const std::vector<VkGeometryNV> geometries =
		{
			model.Procedural()
				? BottomLevelAccelerationStructure::CreateGeometryAabb(scene, aabbOffset, 1, true)
				: BottomLevelAccelerationStructure::CreateGeometry(scene, vertexOffset, vertexCount, indexOffset, indexCount, true)
		};

		bottomAs_.emplace_back(*deviceProcedures_, geometries, false);
		requirements.push_back(bottomAs_.back().GetMemoryRequirements());

		vertexOffset += vertexCount * sizeof(Assets::Vertex);
		indexOffset += indexCount * sizeof(uint32_t);
		aabbOffset += sizeof(glm::vec3) * 2;
	}

	// Allocate the structure memory.
	const auto total = GetTotalRequirements(requirements);

	bottomBuffer_.reset(new Buffer(Device(), total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV));
	bottomBufferMemory_.reset(new DeviceMemory(bottomBuffer_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	bottomScratchBuffer_.reset(new Buffer(Device(), total.Build.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV));
	bottomScratchBufferMemory_.reset(new DeviceMemory(bottomScratchBuffer_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	// Generate the structures.
	VkDeviceSize resultOffset = 0;
	VkDeviceSize scratchOffset = 0;

	for (size_t i = 0; i != bottomAs_.size(); ++i)
	{
		bottomAs_[i].Generate(commandBuffer, *bottomScratchBuffer_, scratchOffset, *bottomBufferMemory_, resultOffset, false);
		resultOffset += requirements[i].Result.size;
		scratchOffset += requirements[i].Build.size;
	}
}

void Application::CreateTopLevelStructures(VkCommandBuffer commandBuffer)
{
	const auto& scene = GetScene();

	// Top level acceleration structure
	std::vector<VkGeometryInstance> geometryInstances;
	std::vector<AccelerationStructure::MemoryRequirements> requirements;

	// Hit group 0: triangles
	// Hit group 1: procedurals
	uint32_t instanceId = 0;

	for (const auto& model : scene.Models())
	{
		geometryInstances.push_back(TopLevelAccelerationStructure::CreateGeometryInstance(
			bottomAs_[instanceId], glm::mat4(1), instanceId, model.Procedural() ? 1 : 0));
		instanceId++;
	}

	topAs_.emplace_back(*deviceProcedures_, geometryInstances, false);
	requirements.push_back(topAs_.back().GetMemoryRequirements());

	// Allocate the structure memory.
	const auto total = GetTotalRequirements(requirements);

	topBuffer_.reset(new Buffer(Device(), total.Result.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV));
	topBufferMemory_.reset(new DeviceMemory(topBuffer_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	topScratchBuffer_.reset(new Buffer(Device(), total.Build.size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV));
	topScratchBufferMemory_.reset(new DeviceMemory(topScratchBuffer_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	const size_t instancesBufferSize = sizeof(VkGeometryInstance) * geometryInstances.size();
	instancesBuffer_.reset(new Buffer(Device(), instancesBufferSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV));
	instancesBufferMemory_.reset(new DeviceMemory(instancesBuffer_->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));

	// Generate the structures.
	topAs_[0].Generate(commandBuffer, *topScratchBuffer_, 0, *topBufferMemory_, 0, *instancesBuffer_, *instancesBufferMemory_, 0, false);
}

void Application::CreateOutputImage()
{
	const auto extent = SwapChain().Extent();
	const auto format = SwapChain().Format();
	const auto tiling = VK_IMAGE_TILING_OPTIMAL;

	accumulationImage_.reset(new Image(Device(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT));
	accumulationImageMemory_.reset(new DeviceMemory(accumulationImage_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	accumulationImageView_.reset(new ImageView(Device(), accumulationImage_->Handle(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT));

	outputImage_.reset(new Image(Device(), extent, format, tiling, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
	outputImageMemory_.reset(new DeviceMemory(outputImage_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	outputImageView_.reset(new ImageView(Device(), outputImage_->Handle(), format, VK_IMAGE_ASPECT_COLOR_BIT));
}

}
