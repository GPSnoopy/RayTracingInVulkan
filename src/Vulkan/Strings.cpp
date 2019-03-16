#include "Strings.hpp"

namespace Vulkan {

const char* Strings::DeviceType(const VkPhysicalDeviceType deviceType)
{
	switch (deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		return "Other";
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return "Integrated GPU";
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return "Discrete GPU";
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return "Virtual GPU";
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return "CPU";
	default:
		return "UnknownDeviceType";
	}
}

const char* Strings::VendorId(const uint32_t vendorId)
{
	switch (vendorId)
	{
	case 0x1002:
		return "AMD";
	case 0x1010:
		return "ImgTec";
	case 0x10DE:
		return "NVIDIA";
	case 0x13B5:
		return "ARM";
	case 0x5143:
		return "Qualcomm";
	case 0x8086:
		return "INTEL";
	default:
		return "UnknownVendor";
	}
}

}
