#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Vertex.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SingleTimeCommands.hpp"

namespace Vulkan::RayTracing {

namespace
{
	VkAccelerationStructureCreateInfoNV GetCreateInfo(const std::vector<VkGeometryNV>& geometries, const bool allowUpdate)
	{
		const auto flags = allowUpdate 
			? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV 
			: VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

		VkAccelerationStructureCreateInfoNV structureInfo = {};
		structureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
		structureInfo.pNext = nullptr;
		structureInfo.compactedSize = 0;
		structureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		structureInfo.info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
		structureInfo.info.flags = flags;
		structureInfo.info.instanceCount = 0; // The bottom-level AS can only contain explicit geometry, and no instances
		structureInfo.info.geometryCount = static_cast<uint32_t>(geometries.size());
		structureInfo.info.pGeometries = geometries.data();

		return structureInfo;
	}
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures, 
	const std::vector<VkGeometryNV>& geometries, 
	const bool allowUpdate) :
	AccelerationStructure(deviceProcedures, GetCreateInfo(geometries, allowUpdate)),
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

	const VkAccelerationStructureNV previousStructure = updateOnly ? Handle() : nullptr;

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
	buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
	buildInfo.instanceCount = 0;
	buildInfo.geometryCount = static_cast<uint32_t>(geometries_.size());
	buildInfo.pGeometries = geometries_.data();

	deviceProcedures_.vkCmdBuildAccelerationStructureNV(
		commandBuffer, &buildInfo, nullptr, 0, updateOnly, Handle(), previousStructure, scratchBuffer.Handle(), scratchOffset);
}

VkGeometryNV BottomLevelAccelerationStructure::CreateGeometry(
	const Assets::Scene& scene,
	const uint32_t vertexOffset, const uint32_t vertexCount,
	const uint32_t indexOffset, const uint32_t indexCount,
	const bool isOpaque)
{
	VkGeometryNV geometry = {};
	geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
	geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
	geometry.geometry.triangles.pNext = nullptr;
	geometry.geometry.triangles.vertexData = scene.VertexBuffer().Handle();
	geometry.geometry.triangles.vertexOffset = vertexOffset;
	geometry.geometry.triangles.vertexCount = vertexCount;
	geometry.geometry.triangles.vertexStride = sizeof(Assets::Vertex);
	geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	geometry.geometry.triangles.indexData = scene.IndexBuffer().Handle();
	geometry.geometry.triangles.indexOffset = indexOffset;
	geometry.geometry.triangles.indexCount = indexCount;
	geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	geometry.geometry.triangles.transformData = nullptr;
	geometry.geometry.triangles.transformOffset = 0;
	geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
	geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_NV : 0;

	return geometry;
}

VkGeometryNV BottomLevelAccelerationStructure::CreateGeometryAabb(
	const Assets::Scene& scene,
	const uint32_t aabbOffset,
	const uint32_t aabbCount,
	const bool isOpaque)
{
	VkGeometryNV geometry = {};
	geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_NV;
	geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
	geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
	geometry.geometry.aabbs.pNext = nullptr;
	geometry.geometry.aabbs.aabbData = scene.AabbBuffer().Handle();
	geometry.geometry.aabbs.numAABBs = aabbCount;
	geometry.geometry.aabbs.stride = sizeof(glm::vec3) * 2;
	geometry.geometry.aabbs.offset = aabbOffset;
	geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_NV : 0;

	return geometry;
}

}
