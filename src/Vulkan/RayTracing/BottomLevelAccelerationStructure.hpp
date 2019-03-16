#pragma once

#include "AccelerationStructure.hpp"
#include <vector>

namespace Assets
{
	class Procedural;
	class Scene;
}

namespace Vulkan::RayTracing
{

	class BottomLevelAccelerationStructure final : public AccelerationStructure
	{
	public:

		BottomLevelAccelerationStructure(const BottomLevelAccelerationStructure&) = delete;
		BottomLevelAccelerationStructure& operator = (const BottomLevelAccelerationStructure&) = delete;
		BottomLevelAccelerationStructure& operator = (BottomLevelAccelerationStructure&&) = delete;

		BottomLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures, const std::vector<VkGeometryNV>& geometries, bool allowUpdate);
		BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept;
		~BottomLevelAccelerationStructure();

		void Generate(
			VkCommandBuffer commandBuffer,
			Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			DeviceMemory& resultMemory,
			VkDeviceSize resultOffset,
			bool updateOnly) const;

		static VkGeometryNV CreateGeometry(
			const Assets::Scene& scene, 
			uint32_t vertexOffset, uint32_t vertexCount,
			uint32_t indexOffset, uint32_t indexCount,
			bool isOpaque);

		static VkGeometryNV CreateGeometryAabb(
			const Assets::Scene& scene,
			uint32_t aabbOffset,
			uint32_t aabbCount,
			bool isOpaque);

	private:

		std::vector<VkGeometryNV> geometries_;
	};

}
