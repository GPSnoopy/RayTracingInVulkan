#include "AccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Device.hpp"
#undef MemoryBarrier

namespace Vulkan::RayTracing {

AccelerationStructure::AccelerationStructure(
	const class DeviceProcedures& deviceProcedures,
	const VkAccelerationStructureTypeKHR accelerationStructureType,
	const std::vector<VkAccelerationStructureCreateGeometryTypeInfoKHR>& geometries,
	const bool allowUpdate) :
	deviceProcedures_(deviceProcedures),
	allowUpdate_(allowUpdate),
	device_(deviceProcedures.Device())
{
	const auto flags = allowUpdate
		? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR
		: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

	VkAccelerationStructureCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.compactedSize = 0;
	createInfo.type = accelerationStructureType;
	createInfo.flags = flags;
	createInfo.maxGeometryCount = static_cast<uint32_t>(geometries.size());
	createInfo.pGeometryInfos = geometries.data();
	createInfo.deviceAddress = 0;
	
	Check(deviceProcedures.vkCreateAccelerationStructureKHR(device_.Handle(), &createInfo, nullptr, &accelerationStructure_), 
		"create acceleration structure");
}

AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept :
	deviceProcedures_(other.deviceProcedures_),
	allowUpdate_(other.allowUpdate_),
	device_(other.device_),
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

AccelerationStructure::MemoryRequirements AccelerationStructure::GetMemoryRequirements() const
{
	VkAccelerationStructureMemoryRequirementsInfoKHR memoryRequirementsInfo{};
	memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
	memoryRequirementsInfo.pNext = nullptr;
	memoryRequirementsInfo.buildType = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
	memoryRequirementsInfo.accelerationStructure = accelerationStructure_;

	// If the descriptor already contains the geometry info, so we can directly compute the estimated size and required scratch memory.
	VkMemoryRequirements2 memoryRequirements = {};
	memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
	memoryRequirements.pNext = nullptr;
	
	memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;
	deviceProcedures_.vkGetAccelerationStructureMemoryRequirementsKHR(device_.Handle(), &memoryRequirementsInfo, &memoryRequirements);
	const auto resultRequirements = memoryRequirements.memoryRequirements;

	memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR;
	deviceProcedures_.vkGetAccelerationStructureMemoryRequirementsKHR(device_.Handle(), &memoryRequirementsInfo, &memoryRequirements);
	const auto buildRequirements = memoryRequirements.memoryRequirements;

	memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_KHR;
	deviceProcedures_.vkGetAccelerationStructureMemoryRequirementsKHR(device_.Handle(), &memoryRequirementsInfo, &memoryRequirements);
	const auto updateRequirements = memoryRequirements.memoryRequirements;

	return { resultRequirements, buildRequirements, updateRequirements };
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
