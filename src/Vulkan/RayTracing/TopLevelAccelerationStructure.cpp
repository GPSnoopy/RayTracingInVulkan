#include "TopLevelAccelerationStructure.hpp"
#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#include <cstring>

namespace Vulkan::RayTracing {

TopLevelAccelerationStructure::TopLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures,
	const class RayTracingProperties& rayTracingProperties,
	const VkDeviceAddress instanceAddress,
	const uint32_t instancesCount) :
	AccelerationStructure(deviceProcedures, rayTracingProperties),
	instancesCount_(instancesCount)
{
	// Create VkAccelerationStructureGeometryInstancesDataKHR. This wraps a device pointer to the above uploaded instances.
	instancesVk_.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	instancesVk_.arrayOfPointers = VK_FALSE;
	instancesVk_.data.deviceAddress = instanceAddress;

	// Put the above into a VkAccelerationStructureGeometryKHR. We need to put the
	// instances struct in a union and label it as instance data.
	topASGeometry_.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	topASGeometry_.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	topASGeometry_.geometry.instances = instancesVk_;
	
	buildGeometryInfo_.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometryInfo_.flags = flags_;
	buildGeometryInfo_.geometryCount = 1;
	buildGeometryInfo_.pGeometries = &topASGeometry_;
	buildGeometryInfo_.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildGeometryInfo_.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildGeometryInfo_.srcAccelerationStructure = nullptr;
	
	buildSizesInfo_ = GetBuildSizes(&instancesCount);
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept :
	AccelerationStructure(std::move(other)),
	instancesCount_(other.instancesCount_)
{
}

TopLevelAccelerationStructure::~TopLevelAccelerationStructure()
{
}

void TopLevelAccelerationStructure::Generate(
	VkCommandBuffer commandBuffer,
	Buffer& scratchBuffer,
	const VkDeviceSize scratchOffset,
	Buffer& resultBuffer,
	const VkDeviceSize resultOffset)
{
	// Create the acceleration structure.
	CreateAccelerationStructure(resultBuffer, resultOffset);

	// Build the actual bottom-level acceleration structure
	VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
	buildOffsetInfo.primitiveCount = instancesCount_;
	
	const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

	buildGeometryInfo_.dstAccelerationStructure = Handle();
	buildGeometryInfo_.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

	deviceProcedures_.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildGeometryInfo_, &pBuildOffsetInfo);
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
