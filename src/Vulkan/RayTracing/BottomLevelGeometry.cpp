#include "BottomLevelGeometry.hpp"
#include "DeviceProcedures.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Vertex.hpp"
#include "Vulkan/Buffer.hpp"

namespace Vulkan::RayTracing {

void BottomLevelGeometry::AddGeometryTriangles(
	const Assets::Scene& scene,
	const uint32_t vertexOffset, const uint32_t vertexCount,
	const uint32_t indexOffset, const uint32_t indexCount,
	const bool isOpaque)
{
	VkAccelerationStructureCreateGeometryTypeInfoKHR createGeometryTypeInfo = {};
	createGeometryTypeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
	createGeometryTypeInfo.pNext = nullptr;
	createGeometryTypeInfo.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	createGeometryTypeInfo.maxPrimitiveCount = indexCount / 3;
	createGeometryTypeInfo.indexType = VK_INDEX_TYPE_UINT32;
	createGeometryTypeInfo.maxVertexCount = vertexCount;
	createGeometryTypeInfo.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	createGeometryTypeInfo.allowsTransforms = false;
	
	VkAccelerationStructureGeometryKHR geometry = {};
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	geometry.geometry.triangles.pNext = nullptr;
	geometry.geometry.triangles.vertexData.deviceAddress = scene.VertexBuffer().GetDeviceAddress();
	geometry.geometry.triangles.vertexStride = sizeof(Assets::Vertex);
	geometry.geometry.triangles.vertexFormat = createGeometryTypeInfo.vertexFormat;
	geometry.geometry.triangles.indexData.deviceAddress = scene.IndexBuffer().GetDeviceAddress();
	geometry.geometry.triangles.indexType = createGeometryTypeInfo.indexType;
	geometry.geometry.triangles.transformData = {};
	geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
	geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

	VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {};
	buildOffsetInfo.firstVertex = vertexOffset / sizeof(Assets::Vertex);
	buildOffsetInfo.primitiveOffset = indexOffset;
	buildOffsetInfo.primitiveCount = indexCount / 3;
	buildOffsetInfo.transformOffset = 0;

	createGeometryTypeInfo_.emplace_back(createGeometryTypeInfo);
	geometry_.emplace_back(geometry);
	buildOffsetInfo_.emplace_back(buildOffsetInfo);
}

void BottomLevelGeometry::AddGeometryAabb(
	const Assets::Scene& scene,
	const uint32_t aabbOffset,
	const uint32_t aabbCount,
	const bool isOpaque)
{
	VkAccelerationStructureCreateGeometryTypeInfoKHR createGeometryTypeInfo = {};
	createGeometryTypeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
	createGeometryTypeInfo.pNext = nullptr;
	createGeometryTypeInfo.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
	createGeometryTypeInfo.maxPrimitiveCount = aabbCount;
	createGeometryTypeInfo.indexType = VK_INDEX_TYPE_NONE_KHR;
	createGeometryTypeInfo.maxVertexCount = 0;
	createGeometryTypeInfo.vertexFormat = VK_FORMAT_UNDEFINED;
	createGeometryTypeInfo.allowsTransforms = false;
	
	VkAccelerationStructureGeometryKHR geometry = {};
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
	geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
	geometry.geometry.aabbs.pNext = nullptr;
	geometry.geometry.aabbs.data.deviceAddress = scene.AabbBuffer().GetDeviceAddress();
	geometry.geometry.aabbs.stride = sizeof(VkAabbPositionsKHR);
	geometry.flags = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

	VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {};
	buildOffsetInfo.firstVertex = 0;
	buildOffsetInfo.primitiveOffset = aabbOffset;
	buildOffsetInfo.primitiveCount = aabbCount;
	buildOffsetInfo.transformOffset = 0;

	createGeometryTypeInfo_.emplace_back(createGeometryTypeInfo);
	geometry_.emplace_back(geometry);
	buildOffsetInfo_.emplace_back(buildOffsetInfo);
}

}
