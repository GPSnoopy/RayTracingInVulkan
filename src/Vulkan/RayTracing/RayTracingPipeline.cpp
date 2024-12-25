#include "RayTracingPipeline.hpp"
#include "DeviceProcedures.hpp"
#include "TopLevelAccelerationStructure.hpp"
#include "Assets/Scene.hpp"
#include "Assets/UniformBuffer.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/DescriptorBinding.hpp"
#include "Vulkan/DescriptorSetManager.hpp"
#include "Vulkan/DescriptorSets.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "Vulkan/SwapChain.hpp"

namespace Vulkan::RayTracing {

RayTracingPipeline::RayTracingPipeline(
	const DeviceProcedures& deviceProcedures,
	const SwapChain& swapChain,
	const TopLevelAccelerationStructure& accelerationStructure,
	const ImageView& accumulationImageView,
	const ImageView& outputImageView,
	const std::vector<Assets::UniformBuffer>& uniformBuffers,
	const Assets::Scene& scene) :
	swapChain_(swapChain)
{
	// Create descriptor pool/sets.
	const auto& device = swapChain.Device();
	const std::vector<DescriptorBinding> descriptorBindings =
	{
		// Top level acceleration structure.
		{0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR},

		// Image accumulation & output
		{1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},
		{2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},

		// Camera information & co
		{3, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR},

		// Vertex buffer, Index buffer, Material buffer, Offset buffer
		{4, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
		{5, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
		{6, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
		{7, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

		// Textures and image samplers
		{8, static_cast<uint32_t>(scene.TextureSamplers().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

		// The Procedural buffer.
		{9, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR}
	};

	descriptorSetManager_.reset(new DescriptorSetManager(device, descriptorBindings, uniformBuffers.size()));

	auto& descriptorSets = descriptorSetManager_->DescriptorSets();

	for (uint32_t i = 0; i != swapChain.Images().size(); ++i)
	{
		// Top level acceleration structure.
		const auto accelerationStructureHandle = accelerationStructure.Handle();
		VkWriteDescriptorSetAccelerationStructureKHR structureInfo = {};
		structureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		structureInfo.pNext = nullptr;
		structureInfo.accelerationStructureCount = 1;
		structureInfo.pAccelerationStructures = &accelerationStructureHandle;

		// Accumulation image
		VkDescriptorImageInfo accumulationImageInfo = {};
		accumulationImageInfo.imageView = accumulationImageView.Handle();
		accumulationImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		// Output image
		VkDescriptorImageInfo outputImageInfo = {};
		outputImageInfo.imageView = outputImageView.Handle();
		outputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		// Uniform buffer
		VkDescriptorBufferInfo uniformBufferInfo = {};
		uniformBufferInfo.buffer = uniformBuffers[i].Buffer().Handle();
		uniformBufferInfo.range = VK_WHOLE_SIZE;

		// Vertex buffer
		VkDescriptorBufferInfo vertexBufferInfo = {};
		vertexBufferInfo.buffer = scene.VertexBuffer().Handle();
		vertexBufferInfo.range = VK_WHOLE_SIZE;

		// Index buffer
		VkDescriptorBufferInfo indexBufferInfo = {};
		indexBufferInfo.buffer = scene.IndexBuffer().Handle();
		indexBufferInfo.range = VK_WHOLE_SIZE;

		// Material buffer
		VkDescriptorBufferInfo materialBufferInfo = {};
		materialBufferInfo.buffer = scene.MaterialBuffer().Handle();
		materialBufferInfo.range = VK_WHOLE_SIZE;

		// Offsets buffer
		VkDescriptorBufferInfo offsetsBufferInfo = {};
		offsetsBufferInfo.buffer = scene.OffsetsBuffer().Handle();
		offsetsBufferInfo.range = VK_WHOLE_SIZE;

		// Image and texture samplers.
		std::vector<VkDescriptorImageInfo> imageInfos(scene.TextureSamplers().size());

		for (size_t t = 0; t != imageInfos.size(); ++t)
		{
			auto& imageInfo = imageInfos[t];
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = scene.TextureImageViews()[t];
			imageInfo.sampler = scene.TextureSamplers()[t];
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites =
		{
			descriptorSets.Bind(i, 0, structureInfo),
			descriptorSets.Bind(i, 1, accumulationImageInfo),
			descriptorSets.Bind(i, 2, outputImageInfo),
			descriptorSets.Bind(i, 3, uniformBufferInfo),
			descriptorSets.Bind(i, 4, vertexBufferInfo),
			descriptorSets.Bind(i, 5, indexBufferInfo),
			descriptorSets.Bind(i, 6, materialBufferInfo),
			descriptorSets.Bind(i, 7, offsetsBufferInfo),
			descriptorSets.Bind(i, 8, *imageInfos.data(), static_cast<uint32_t>(imageInfos.size()))
		};

		// Procedural buffer (optional)
		VkDescriptorBufferInfo proceduralBufferInfo = {};
		
		if (scene.HasProcedurals())
		{
			proceduralBufferInfo.buffer = scene.ProceduralBuffer().Handle();
			proceduralBufferInfo.range = VK_WHOLE_SIZE;

			descriptorWrites.push_back(descriptorSets.Bind(i, 9, proceduralBufferInfo));
		}

		descriptorSets.UpdateDescriptors(descriptorWrites);
	}

	pipelineLayout_.reset(new class PipelineLayout(device, descriptorSetManager_->DescriptorSetLayout()));

	// Load shaders.
	const ShaderModule rayGenShader(device, "../assets/shaders/RayTracing.rgen.spv");
	const ShaderModule missShader(device, "../assets/shaders/RayTracing.rmiss.spv");
	const ShaderModule closestHitShader(device, "../assets/shaders/RayTracing.rchit.spv");
	const ShaderModule proceduralClosestHitShader(device, "../assets/shaders/RayTracing.Procedural.rchit.spv");
	const ShaderModule proceduralIntersectionShader(device, "../assets/shaders/RayTracing.Procedural.rint.spv");

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
	{
		rayGenShader.CreateShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
		missShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
		closestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
		proceduralClosestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
		proceduralIntersectionShader.CreateShaderStage(VK_SHADER_STAGE_INTERSECTION_BIT_KHR)
	};

	// Shader groups
	VkRayTracingShaderGroupCreateInfoKHR rayGenGroupInfo = {};
	rayGenGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	rayGenGroupInfo.pNext = nullptr;
	rayGenGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	rayGenGroupInfo.generalShader = 0;
	rayGenGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
	rayGenGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	rayGenGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	rayGenIndex_ = 0;

	VkRayTracingShaderGroupCreateInfoKHR missGroupInfo = {};
	missGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	missGroupInfo.pNext = nullptr;
	missGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	missGroupInfo.generalShader = 1;
	missGroupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
	missGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	missGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	missIndex_ = 1;

	VkRayTracingShaderGroupCreateInfoKHR triangleHitGroupInfo = {};
	triangleHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	triangleHitGroupInfo.pNext = nullptr;
	triangleHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	triangleHitGroupInfo.generalShader = VK_SHADER_UNUSED_KHR;
	triangleHitGroupInfo.closestHitShader = 2;
	triangleHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	triangleHitGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	triangleHitGroupIndex_ = 2;

	VkRayTracingShaderGroupCreateInfoKHR proceduralHitGroupInfo = {};
	proceduralHitGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	proceduralHitGroupInfo.pNext = nullptr;
	proceduralHitGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
	proceduralHitGroupInfo.generalShader = VK_SHADER_UNUSED_KHR;
	proceduralHitGroupInfo.closestHitShader = 3;
	proceduralHitGroupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	proceduralHitGroupInfo.intersectionShader = 4;
	proceduralHitGroupIndex_ = 3;

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups =
	{
		rayGenGroupInfo, 
		missGroupInfo, 
		triangleHitGroupInfo, 
		proceduralHitGroupInfo,
	};

	// Create graphic pipeline
	VkRayTracingPipelineCreateInfoKHR pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.groupCount = static_cast<uint32_t>(groups.size());
	pipelineInfo.pGroups = groups.data();
	pipelineInfo.maxPipelineRayRecursionDepth = 1;
	pipelineInfo.layout = pipelineLayout_->Handle();
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.basePipelineIndex = 0;

	Check(deviceProcedures.vkCreateRayTracingPipelinesKHR(device.Handle(), nullptr, nullptr, 1, &pipelineInfo, nullptr, &pipeline_), 
		"create ray tracing pipeline");
}

RayTracingPipeline::~RayTracingPipeline()
{
	if (pipeline_ != nullptr)
	{
		vkDestroyPipeline(swapChain_.Device().Handle(), pipeline_, nullptr);
		pipeline_ = nullptr;
	}

	pipelineLayout_.reset();
	descriptorSetManager_.reset();
}

VkDescriptorSet RayTracingPipeline::DescriptorSet(const size_t index) const
{
	return descriptorSetManager_->DescriptorSets().Handle(index);
}

}
