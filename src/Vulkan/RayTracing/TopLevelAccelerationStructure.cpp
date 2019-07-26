#include "TopLevelAccelerationStructure.hpp"
#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/DeviceMemory.hpp"
#include <cstring>

namespace Vulkan::RayTracing {

namespace
{
	VkAccelerationStructureCreateInfoNV GetCreateInfo(const size_t instanceCount, const bool allowUpdate)
	{
		const auto flags = allowUpdate
			? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV
			: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

		VkAccelerationStructureCreateInfoNV structureInfo = {};
		structureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
		structureInfo.pNext = nullptr;
		structureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		structureInfo.info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
		structureInfo.info.flags = flags;
		structureInfo.compactedSize = 0;
		structureInfo.info.instanceCount = static_cast<uint32_t>(instanceCount);
		structureInfo.info.geometryCount = 0; // Since this is a top-level AS, it does not contain any geometry
		structureInfo.info.pGeometries = nullptr;

		return structureInfo;
	}
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures,
	const std::vector<VkGeometryInstance>& geometryInstances,
	const bool allowUpdate) :
	AccelerationStructure(deviceProcedures, GetCreateInfo(geometryInstances.size(), allowUpdate)),
	geometryInstances_(geometryInstances)
{
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept :
	AccelerationStructure(std::move(other)),
	geometryInstances_(std::move(other.geometryInstances_))
{
}

TopLevelAccelerationStructure::~TopLevelAccelerationStructure()
{
}

void TopLevelAccelerationStructure::Generate(
	VkCommandBuffer commandBuffer,
	Buffer& scratchBuffer,
	const VkDeviceSize scratchOffset,
	DeviceMemory& resultMemory,
	const VkDeviceSize resultOffset,
	Buffer& instanceBuffer,
	DeviceMemory& instanceMemory,
	const VkDeviceSize instanceOffset,
	const bool updateOnly) const
{
	if (updateOnly && !allowUpdate_)
	{
		throw std::invalid_argument("cannot update readonly structure");
	}

	const VkAccelerationStructureNV previousStructure = updateOnly ? Handle() : nullptr;

	// Copy the instance descriptors into the provider buffer.
	const auto instancesBufferSize = geometryInstances_.size() * sizeof(VkGeometryInstance);
	void* data = instanceMemory.Map(0, instancesBufferSize);
	std::memcpy(data, geometryInstances_.data(), instancesBufferSize);

	// Bind the acceleration structure descriptor to the actual memory that will contain it
	VkBindAccelerationStructureMemoryInfoNV bindInfo = {};
	bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
	bindInfo.pNext = nullptr;
	bindInfo.accelerationStructure = Handle();
	bindInfo.memory = resultMemory.Handle();
	bindInfo.memoryOffset = resultOffset;
	bindInfo.deviceIndexCount = 0;
	bindInfo.pDeviceIndices = nullptr;

	Check(deviceProcedures_.vkBindAccelerationStructureMemoryNV(Device().Handle(), 1, &bindInfo),
		"bind acceleration structure");

	// Build the actual bottom-level acceleration structure
	const auto flags = allowUpdate_ 
		? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV 
		: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

	VkAccelerationStructureInfoNV buildInfo = {};
	buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
	buildInfo.pNext = nullptr;
	buildInfo.flags = flags;
	buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
	buildInfo.instanceCount = static_cast<uint32_t>(geometryInstances_.size());
	buildInfo.geometryCount = 0;
	buildInfo.pGeometries = nullptr;

	deviceProcedures_.vkCmdBuildAccelerationStructureNV(
		commandBuffer, &buildInfo, instanceBuffer.Handle(), instanceOffset, updateOnly, Handle(), previousStructure, scratchBuffer.Handle(), scratchOffset);
}

VkGeometryInstance TopLevelAccelerationStructure::CreateGeometryInstance(
	const BottomLevelAccelerationStructure& bottomLevelAs,
	const glm::mat4& transform,
	const uint32_t instanceId,
	const uint32_t hitGroupIndex)
{
	const auto& device = bottomLevelAs.Device();
	const auto& deviceProcedures = bottomLevelAs.DeviceProcedures();

	uint64_t accelerationStructureHandle;
	Check(deviceProcedures.vkGetAccelerationStructureHandleNV(device.Handle(), bottomLevelAs.Handle(), sizeof(uint64_t), &accelerationStructureHandle),
		"get acceleration structure handle");

	VkGeometryInstance geometryInstance = {};
	std::memcpy(geometryInstance.transform, &transform, sizeof(glm::mat4));
	geometryInstance.instanceCustomIndex = instanceId;
	geometryInstance.mask = 0xFF; // The visibility mask is always set of 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
	geometryInstance.instanceOffset = hitGroupIndex; // Set the hit group index, that will be used to find the shader code to execute when hitting the geometry.
	geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV; // Disable culling - more fine control could be provided by the application
	geometryInstance.accelerationStructureHandle = accelerationStructureHandle;

	return geometryInstance;
}

}
