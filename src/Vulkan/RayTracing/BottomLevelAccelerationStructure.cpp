#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Vertex.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"

namespace Vulkan::RayTracing {

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures, 
	const BottomLevelGeometry& geometries,
	const bool allowUpdate) :
	AccelerationStructure(deviceProcedures, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, geometries.CreateGeometryTypeInfo(), allowUpdate),
	geometries_(geometries)
{
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept :
	AccelerationStructure(std::move(other)),
	geometries_(std::move(other.geometries_))
{
}

BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure()
{
}

void BottomLevelAccelerationStructure::Generate(
	VkCommandBuffer commandBuffer, 
	Buffer& scratchBuffer,
	const VkDeviceSize scratchOffset,
	DeviceMemory& resultMemory,
	const VkDeviceSize resultOffset,
	const bool updateOnly) const
{
	if (updateOnly && !allowUpdate_)
	{
		throw std::invalid_argument("cannot update readonly structure");
	}

	const VkAccelerationStructureKHR previousStructure = updateOnly ? Handle() : nullptr;

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

	// Build the actual bottom-level acceleration structure
	const auto flags = allowUpdate_ 
		? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR 
		: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

	const VkAccelerationStructureGeometryKHR* pGeometry = geometries_.Geometry().data();
	const VkAccelerationStructureBuildOffsetInfoKHR* pBuildOffsetInfo = geometries_.BuildOffsetInfo().data();
	
	VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
	buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildInfo.pNext = nullptr;
	buildInfo.flags = flags;
	buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	buildInfo.update = updateOnly;
	buildInfo.srcAccelerationStructure = previousStructure;
	buildInfo.dstAccelerationStructure = Handle(),
	buildInfo.geometryArrayOfPointers = false;
	buildInfo.geometryCount = static_cast<uint32_t>(geometries_.size());
	buildInfo.ppGeometries = &pGeometry;
	buildInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

	deviceProcedures_.vkCmdBuildAccelerationStructureKHR(commandBuffer, 1, &buildInfo, &pBuildOffsetInfo);
}

}
