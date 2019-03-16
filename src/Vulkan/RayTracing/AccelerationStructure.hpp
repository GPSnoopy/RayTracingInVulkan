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

		struct MemoryRequirements
		{
			VkMemoryRequirements Result;
			VkMemoryRequirements Build;
			VkMemoryRequirements Update;
		};

		AccelerationStructure(const AccelerationStructure&) = delete;
		AccelerationStructure& operator = (const AccelerationStructure&) = delete;
		AccelerationStructure& operator = (AccelerationStructure&&) = delete;

		AccelerationStructure(AccelerationStructure&& other) noexcept;
		virtual ~AccelerationStructure();

		const class Device& Device() const { return device_; }
		const class DeviceProcedures& DeviceProcedures() const { return deviceProcedures_; }

		MemoryRequirements GetMemoryRequirements() const;
		
		static void MemoryBarrier(VkCommandBuffer commandBuffer);

	protected:

		AccelerationStructure(const class DeviceProcedures& deviceProcedures, const VkAccelerationStructureCreateInfoNV& createInfo);
		
		const class DeviceProcedures& deviceProcedures_;
		const bool allowUpdate_;

	private:

		const class Device& device_;

		VULKAN_HANDLE(VkAccelerationStructureNV, accelerationStructure_)
	};

}
