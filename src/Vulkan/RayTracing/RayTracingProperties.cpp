#include "RayTracingProperties.hpp"
#include "Vulkan/Device.hpp"

namespace Vulkan::RayTracing {

RayTracingProperties::RayTracingProperties(const class Device& device) :
	device_(device)
{
	props_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

	VkPhysicalDeviceProperties2 props = {};
	props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	props.pNext = &props_;
	vkGetPhysicalDeviceProperties2(device.PhysicalDevice(), &props);
}

}
