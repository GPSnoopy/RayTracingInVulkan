#include "RayTracingProperties.hpp"
#include "Vulkan/Device.hpp"

namespace Vulkan::RayTracing {

RayTracingProperties::RayTracingProperties(const class Device& device) :
	device_(device)
{
	accelProps_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
	pipelineProps_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	pipelineProps_.pNext = &accelProps_;

	VkPhysicalDeviceProperties2 props = {};
	props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	props.pNext = &pipelineProps_;
	vkGetPhysicalDeviceProperties2(device.PhysicalDevice(), &props);
}

}
