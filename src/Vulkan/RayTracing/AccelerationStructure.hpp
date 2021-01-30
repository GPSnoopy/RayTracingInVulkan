#pragma once

#include "Vulkan/Vulkan.hpp"

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
}

namespace Vulkan::RayTracing
{
	class DeviceProcedures;

	class AccelerationStructure
	{
	public:

		AccelerationStructure(const AccelerationStructure&) = delete;
		AccelerationStructure& operator = (const AccelerationStructure&) = delete;
		AccelerationStructure& operator = (AccelerationStructure&&) = delete;

		AccelerationStructure(AccelerationStructure&& other) noexcept;
		virtual ~AccelerationStructure();

		const class Device& Device() const { return device_; }
		const class DeviceProcedures& DeviceProcedures() const { return deviceProcedures_; }
		const VkAccelerationStructureBuildSizesInfoKHR BuildSizes() const { return buildSizesInfo_; }

		static void MemoryBarrier(VkCommandBuffer commandBuffer);
	
	protected:

		explicit AccelerationStructure(const class DeviceProcedures& deviceProcedures, const class RayTracingProperties& rayTracingProperties);

		VkAccelerationStructureBuildSizesInfoKHR GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const;
		void CreateAccelerationStructure(Buffer& resultBuffer, VkDeviceSize resultOffset);

		const class DeviceProcedures& deviceProcedures_;
		const VkBuildAccelerationStructureFlagsKHR flags_;

		VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo_{};
		VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo_{};

	private:

		const class Device& device_;
		const class RayTracingProperties& rayTracingProperties_;
		
		VULKAN_HANDLE(VkAccelerationStructureKHR, accelerationStructure_)
	};

}
