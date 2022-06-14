#include "ShaderBindingTable.hpp"
#include "DeviceProcedures.hpp"
#include "RayTracingPipeline.hpp"
#include "RayTracingProperties.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/DeviceMemory.hpp"
#include <algorithm>
#include <cstring>

namespace Vulkan::RayTracing {

namespace
{
	size_t RoundUp(size_t size, size_t powerOf2Alignment)
	{
		return (size+powerOf2Alignment-1) & ~(powerOf2Alignment-1);
	}

	size_t GetEntrySize(const RayTracingProperties& rayTracingProperties, const std::vector<ShaderBindingTable::Entry>& entries)
	{
		// Find the maximum number of parameters used by a single entry
		size_t maxArgs = 0;

		for (const auto& entry : entries)
		{
			maxArgs = std::max(maxArgs, entry.InlineData.size());
		}

		// A SBT entry is made of a program ID and a set of 4-byte parameters (see shaderRecordEXT).
		// Its size is ShaderGroupHandleSize (plus parameters) and must be aligned to ShaderGroupBaseAlignment.
		return RoundUp(rayTracingProperties.ShaderGroupHandleSize() + maxArgs, rayTracingProperties.ShaderGroupBaseAlignment());
	}

	size_t CopyShaderData(
		uint8_t* const dst, 
		const RayTracingProperties& rayTracingProperties, 
		const std::vector<ShaderBindingTable::Entry>& entries, 
		const size_t entrySize, 
		const uint8_t* const shaderHandleStorage)
	{
		const auto handleSize = rayTracingProperties.ShaderGroupHandleSize();

		uint8_t* pDst = dst;

		for (const auto& entry : entries)
		{
			// Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesKHR.
			std::memcpy(pDst, shaderHandleStorage + entry.GroupIndex * handleSize, handleSize);
			std::memcpy(pDst + handleSize, entry.InlineData.data(), entry.InlineData.size());

			pDst += entrySize;
		}

		return entries.size() * entrySize;
	}

}

ShaderBindingTable::ShaderBindingTable(
	const DeviceProcedures& deviceProcedures, 
	const RayTracingPipeline& rayTracingPipeline,
	const RayTracingProperties& rayTracingProperties,
	const std::vector<Entry>& rayGenPrograms,
	const std::vector<Entry>& missPrograms, 
	const std::vector<Entry>& hitGroups) :
	
	rayGenEntrySize_(GetEntrySize(rayTracingProperties, rayGenPrograms)),
	missEntrySize_(GetEntrySize(rayTracingProperties, missPrograms)),
	hitGroupEntrySize_(GetEntrySize(rayTracingProperties, hitGroups)),
	
	rayGenOffset_(0),
	missOffset_(rayGenPrograms.size() * rayGenEntrySize_),
	hitGroupOffset_(missOffset_ + missPrograms.size() * missEntrySize_),

	rayGenSize_(rayGenPrograms.size() * rayGenEntrySize_),
	missSize_(missPrograms.size() * missEntrySize_),
	hitGroupSize_(hitGroups.size() * hitGroupEntrySize_)
{
	// Compute the size of the table.
	const size_t sbtSize =
		rayGenPrograms.size() * rayGenEntrySize_ +
		missPrograms.size() * missEntrySize_ +
		hitGroups.size() * hitGroupEntrySize_;

	// Allocate buffer & memory.
	const auto& device = rayTracingProperties.Device();

	buffer_.reset(new class Buffer(device, sbtSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR));
	bufferMemory_.reset(new DeviceMemory(buffer_->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)));

	// Generate the table.
	const uint32_t handleSize = rayTracingProperties.ShaderGroupHandleSize();
	const size_t groupCount = rayGenPrograms.size() + missPrograms.size() + hitGroups.size();
	std::vector<uint8_t> shaderHandleStorage(groupCount * handleSize);

	Check(deviceProcedures.vkGetRayTracingShaderGroupHandlesKHR(
		device.Handle(), 
		rayTracingPipeline.Handle(), 
		0, static_cast<uint32_t>(groupCount),
		shaderHandleStorage.size(),
		shaderHandleStorage.data()), 
		"get ray tracing shader group handles");

	// Copy the shader identifiers followed by their resource pointers or root constants: 
	// first the ray generation, then the miss shaders, and finally the set of hit groups.
	auto* pData = static_cast<uint8_t*>(bufferMemory_->Map(0, sbtSize));

	pData += CopyShaderData(pData, rayTracingProperties, rayGenPrograms, rayGenEntrySize_, shaderHandleStorage.data());
	pData += CopyShaderData(pData, rayTracingProperties, missPrograms, missEntrySize_, shaderHandleStorage.data());
	         CopyShaderData(pData, rayTracingProperties, hitGroups, hitGroupEntrySize_, shaderHandleStorage.data());

	bufferMemory_->Unmap();
}

ShaderBindingTable::~ShaderBindingTable()
{
	buffer_.reset();
	bufferMemory_.reset();
}

}
