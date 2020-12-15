#pragma once

#include "Vulkan/Vulkan.hpp"
#include <vector>

namespace Assets
{
	class Procedural;
	class Scene;
}

namespace Vulkan::RayTracing
{

	class BottomLevelGeometry final
	{
	public:

		size_t size() const { return geometry_.size(); }
		
		const std::vector<VkAccelerationStructureGeometryKHR>& Geometry() const { return geometry_; }
		const std::vector<VkAccelerationStructureBuildRangeInfoKHR>& BuildOffsetInfo() const { return buildOffsetInfo_; }

		void AddGeometryTriangles(
			const Assets::Scene& scene,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			uint32_t indexOffset, 
			uint32_t indexCount,
			bool isOpaque);

		void AddGeometryAabb(
			const Assets::Scene& scene,
			uint32_t aabbOffset,
			uint32_t aabbCount,
			bool isOpaque);

	private:

		// The geometry to build, addresses of vertices and indices.
		std::vector<VkAccelerationStructureGeometryKHR> geometry_;
		
		// the number of elements to build and offsets
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildOffsetInfo_;
	};

}
