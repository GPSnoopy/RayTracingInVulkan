#pragma once

#include "AccelerationStructure.hpp"
#include "BottomLevelGeometry.hpp"

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

		BottomLevelAccelerationStructure(
			const class DeviceProcedures& deviceProcedures, 
			const class RayTracingProperties& rayTracingProperties, 
			const BottomLevelGeometry& geometries);
		BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept;
		~BottomLevelAccelerationStructure();

		void Generate(
			VkCommandBuffer commandBuffer,
			Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			Buffer& resultBuffer,
			VkDeviceSize resultOffset);

	private:

		BottomLevelGeometry geometries_;
	};

}
