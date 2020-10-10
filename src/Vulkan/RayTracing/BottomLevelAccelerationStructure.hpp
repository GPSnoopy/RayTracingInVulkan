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

		BottomLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures, const BottomLevelGeometry& geometries, bool allowUpdate);
		BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept;
		~BottomLevelAccelerationStructure();

		void Generate(
			VkCommandBuffer commandBuffer,
			Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			DeviceMemory& resultMemory,
			VkDeviceSize resultOffset,
			bool updateOnly) const;

	private:

		BottomLevelGeometry geometries_;
	};

}
