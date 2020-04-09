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
	std::vector<VkAccelerationStructureCreateGeometryTypeInfoKHR> GetCreateGeometryTypeInfo(const size_t instanceCount)
	{

		VkAccelerationStructureCreateGeometryTypeInfoKHR createGeometryTypeInfo = {};
		createGeometryTypeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
		createGeometryTypeInfo.pNext = nullptr;
		createGeometryTypeInfo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		createGeometryTypeInfo.maxPrimitiveCount = static_cast<uint32_t>(instanceCount);
		createGeometryTypeInfo.allowsTransforms = true;

		return std::vector<VkAccelerationStructureCreateGeometryTypeInfoKHR>{createGeometryTypeInfo};
	}
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures,
	const std::vector<VkAccelerationStructureInstanceKHR>& instances,
	const bool allowUpdate) :
	AccelerationStructure(deviceProcedures, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, GetCreateGeometryTypeInfo(instances.size()), allowUpdate),
	instances_(instances)
{
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept :
	AccelerationStructure(std::move(other)),
	instances_(std::move(other.instances_))
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

	const VkAccelerationStructureKHR previousStructure = updateOnly ? Handle() : nullptr;

	// Copy the instance descriptors into the provider buffer.
	const auto instancesBufferSize = instances_.size() * sizeof(VkAccelerationStructureInstanceKHR);
	void* data = instanceMemory.Map(0, instancesBufferSize);
	std::memcpy(data, instances_.data(), instancesBufferSize);

	// Bind the acceleration structure descriptor to the actual memory that will contain it
	VkBindAccelerationStructureMemoryInfoKHR bindInfo = {};
	bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR;
	bindInfo.pNext = nullptr;
	bindInfo.accelerationStructure = Handle();
	bindInfo.memory = resultMemory.Handle();
	bindInfo.memoryOffset = resultOffset;
	bindInfo.deviceIndexCount = 0;
	bindInfo.pDeviceIndices = nullptr;

	Check(deviceProcedures_.vkBindAccelerationStructureMemoryKHR(Device().Handle(), 1, &bindInfo),
		"bind acceleration structure");

	// Create instance geometry structures
	VkAccelerationStructureGeometryKHR geometry = {};
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	geometry.geometry.instances.arrayOfPointers = false;
	geometry.geometry.instances.data.deviceAddress = instanceBuffer.GetDeviceAddress() + instanceOffset;

	VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {};
	buildOffsetInfo.primitiveCount = static_cast<uint32_t>(instances_.size());

	// Build the actual bottom-level acceleration structure
	const auto flags = allowUpdate_
		? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR
		: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	
	const VkAccelerationStructureGeometryKHR* pGeometry = &geometry;
	const VkAccelerationStructureBuildOffsetInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

	VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
	buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildInfo.pNext = nullptr;
	buildInfo.flags = flags;
	buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildInfo.update = updateOnly;
	buildInfo.srcAccelerationStructure = previousStructure;
	buildInfo.dstAccelerationStructure = Handle(),
	buildInfo.geometryArrayOfPointers = false;
	buildInfo.geometryCount = 1;
	buildInfo.ppGeometries = &pGeometry;
	buildInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

	deviceProcedures_.vkCmdBuildAccelerationStructureKHR(commandBuffer, 1, &buildInfo, &pBuildOffsetInfo);
}

VkAccelerationStructureInstanceKHR TopLevelAccelerationStructure::CreateInstance(
	const BottomLevelAccelerationStructure& bottomLevelAs,
	const glm::mat4& transform,
	const uint32_t instanceId,
	const uint32_t hitGroupId)
{
	const auto& device = bottomLevelAs.Device();
	const auto& deviceProcedure = bottomLevelAs.DeviceProcedures();

	VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
	addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	addressInfo.pNext = nullptr;
	addressInfo.accelerationStructure = bottomLevelAs.Handle();
	
	const VkDeviceAddress address = deviceProcedure.vkGetAccelerationStructureDeviceAddressKHR(device.Handle(), &addressInfo);

	VkAccelerationStructureInstanceKHR instance = {};
	instance.instanceCustomIndex = instanceId;
	instance.mask = 0xFF; // The visibility mask is always set of 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
	instance.instanceShaderBindingTableRecordOffset = hitGroupId; // Set the hit group index, that will be used to find the shader code to execute when hitting the geometry.
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; // Disable culling - more fine control could be provided by the application
	instance.accelerationStructureReference = address;

	// The instance.transform value only contains 12 values, corresponding to a 4x3 matrix,
	// hence saving the last row that is anyway always (0,0,0,1).
	// Since the matrix is row-major, we simply copy the first 12 values of the original 4x4 matrix
	std::memcpy(&instance.transform, &transform, sizeof(instance.transform));

	return instance;
}

}
