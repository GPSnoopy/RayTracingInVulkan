#pragma once

#include "AccelerationStructure.hpp"
#include "Utilities/Glm.hpp"
#include <vector>

namespace Vulkan::RayTracing
{
	/// Geometry instance, with the layout expected by VK_NV_ray_tracing
	struct VkGeometryInstance
	{
		/// Transform matrix, containing only the top 3 rows
		float transform[12];
		/// Instance index
		uint32_t instanceCustomIndex : 24;
		/// Visibility mask
		uint32_t mask : 8;
		/// Index of the hit group which will be invoked when a ray hits the instance
		uint32_t instanceOffset : 24;
		/// Instance flags, such as culling
		uint32_t flags : 8;
		/// Opaque handle of the bottom-level acceleration structure
		uint64_t accelerationStructureHandle;
	};

	class BottomLevelAccelerationStructure;

	class TopLevelAccelerationStructure final : public AccelerationStructure
	{
	public:

		TopLevelAccelerationStructure(const TopLevelAccelerationStructure&) = delete;
		TopLevelAccelerationStructure& operator = (const TopLevelAccelerationStructure&) = delete;
		TopLevelAccelerationStructure& operator = (TopLevelAccelerationStructure&&) = delete;

		TopLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures, const std::vector<VkGeometryInstance>& geometryInstances, bool allowUpdate);
		TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept;
		virtual ~TopLevelAccelerationStructure();

		const std::vector<VkGeometryInstance>& GeometryInstances() const { return geometryInstances_; }

		void Generate(
			VkCommandBuffer commandBuffer,
			Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			DeviceMemory& resultMemory,
			VkDeviceSize resultOffset,
			Buffer& instanceBuffer,
			DeviceMemory& instanceMemory,
			VkDeviceSize instanceOffset,
			bool updateOnly) const;

		static VkGeometryInstance CreateGeometryInstance(
			const BottomLevelAccelerationStructure& bottomLevelAs,
			const glm::mat4& transform,
			uint32_t instanceId,
			uint32_t hitGroupIndex);

	private:

		std::vector<VkGeometryInstance> geometryInstances_;
	};

}
