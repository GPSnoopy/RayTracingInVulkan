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

		TopLevelAccelerationStructure(
			const class DeviceProcedures& deviceProcedures,
			const class RayTracingProperties& rayTracingProperties,
			VkDeviceAddress instanceAddress, 
			uint32_t instancesCount);
		TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept;
		virtual ~TopLevelAccelerationStructure();

		void Generate(
			VkCommandBuffer commandBuffer,
			Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			Buffer& resultBuffer,
			VkDeviceSize resultOffset);

		static VkAccelerationStructureInstanceKHR CreateInstance(
			const BottomLevelAccelerationStructure& bottomLevelAs,
			const glm::mat4& transform,
			uint32_t instanceId,
			uint32_t hitGroupId);

	private:

		uint32_t instancesCount_;
		VkAccelerationStructureGeometryInstancesDataKHR instancesVk_{};
		VkAccelerationStructureGeometryKHR topASGeometry_{};
	};

}
