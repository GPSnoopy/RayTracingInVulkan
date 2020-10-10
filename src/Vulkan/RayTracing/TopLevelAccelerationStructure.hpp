#pragma once

#include "AccelerationStructure.hpp"
#include "Utilities/Glm.hpp"
#include <vector>

namespace Vulkan::RayTracing
{
	class BottomLevelAccelerationStructure;

	class TopLevelAccelerationStructure final : public AccelerationStructure
	{
	public:

		TopLevelAccelerationStructure(const TopLevelAccelerationStructure&) = delete;
		TopLevelAccelerationStructure& operator = (const TopLevelAccelerationStructure&) = delete;
		TopLevelAccelerationStructure& operator = (TopLevelAccelerationStructure&&) = delete;

		TopLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures, const std::vector<VkAccelerationStructureInstanceKHR>& instances, bool allowUpdate);
		TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept;
		virtual ~TopLevelAccelerationStructure();

		//const std::vector<TopLevelInstance>& GeometryInstances() const { return geometryInstances_; }

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

		static VkAccelerationStructureInstanceKHR CreateInstance(
			const BottomLevelAccelerationStructure& bottomLevelAs,
			const glm::mat4& transform,
			uint32_t instanceId,
			uint32_t hitGroupId);

	private:

		std::vector<VkAccelerationStructureInstanceKHR> instances_;
	};

}
