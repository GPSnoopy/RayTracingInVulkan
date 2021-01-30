#include "AccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "RayTracingProperties.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#undef MemoryBarrier

namespace Vulkan::RayTracing {

namespace
{
	uint64_t RoundUp(uint64_t size, uint64_t granularity)
	{
		const auto divUp = (size + granularity - 1) / granularity;
		return divUp * granularity;
	}
}

AccelerationStructure::AccelerationStructure(const class DeviceProcedures& deviceProcedures, const RayTracingProperties& rayTracingProperties) :
	deviceProcedures_(deviceProcedures),
	flags_(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR),
	device_(deviceProcedures.Device()),
	rayTracingProperties_(rayTracingProperties)
{
}

AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept :
	deviceProcedures_(other.deviceProcedures_),
	flags_(other.flags_),
	buildGeometryInfo_(other.buildGeometryInfo_),
	buildSizesInfo_(other.buildSizesInfo_),
	device_(other.device_),
	rayTracingProperties_(other.rayTracingProperties_),
	accelerationStructure_(other.accelerationStructure_)
{
	other.accelerationStructure_ = nullptr;
}

AccelerationStructure::~AccelerationStructure()
{
	if (accelerationStructure_ != nullptr)
	{
		deviceProcedures_.vkDestroyAccelerationStructureKHR(device_.Handle(), accelerationStructure_, nullptr);
		accelerationStructure_ = nullptr;
	}
}

VkAccelerationStructureBuildSizesInfoKHR AccelerationStructure::GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const
{
	// Query both the size of the finished acceleration structure and the amount of scratch memory needed.
	VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
	sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

	deviceProcedures_.vkGetAccelerationStructureBuildSizesKHR(
		device_.Handle(), 
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&buildGeometryInfo_,
		pMaxPrimitiveCounts,
		&sizeInfo);

	// AccelerationStructure offset needs to be 256 bytes aligned (official Vulkan specs, don't ask me why).
	const uint64_t AccelerationStructureAlignment = 256;
	const uint64_t ScratchAlignment = rayTracingProperties_.MinAccelerationStructureScratchOffsetAlignment();

	sizeInfo.accelerationStructureSize = RoundUp(sizeInfo.accelerationStructureSize, AccelerationStructureAlignment);
	sizeInfo.buildScratchSize = RoundUp(sizeInfo.buildScratchSize, ScratchAlignment);
	
	return sizeInfo;
}

void AccelerationStructure::CreateAccelerationStructure(Buffer& resultBuffer, const VkDeviceSize resultOffset)
{
	VkAccelerationStructureCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.type = buildGeometryInfo_.type;
	createInfo.size = BuildSizes().accelerationStructureSize;
	createInfo.buffer = resultBuffer.Handle();
	createInfo.offset = resultOffset;

	Check(deviceProcedures_.vkCreateAccelerationStructureKHR(device_.Handle(), &createInfo, nullptr, &accelerationStructure_),
		"create acceleration structure");
}

void AccelerationStructure::MemoryBarrier(VkCommandBuffer commandBuffer)
{
	// Wait for the builder to complete by setting a barrier on the resulting buffer. This is
	// particularly important as the construction of the top-level hierarchy may be called right
	// afterwards, before executing the command list.
	VkMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
	memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
}

}
