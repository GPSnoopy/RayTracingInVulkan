#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{

	class Strings final
	{
	public:

		VULKAN_NON_COPIABLE(Strings)
		
		Strings() = delete;
		~Strings() = delete;

		static const char* DeviceType(VkPhysicalDeviceType deviceType);
		static const char* VendorId(uint32_t vendorId);
	};

}
