#pragma once

#include "Vulkan.hpp"
#include <ostream>

namespace Vulkan 
{

	class Version final
	{
	public:

		explicit Version(const uint32_t version) :
			Major(VK_VERSION_MAJOR(version)),
			Minor(VK_VERSION_MINOR(version)),
			Patch(VK_VERSION_PATCH(version))
		{
		}

		Version(const uint32_t version, const uint32_t vendorId) :
			Major(VK_VERSION_MAJOR(version)),
			Minor(VK_VERSION_MINOR(version) >> (vendorId == 0x10DE ? 2 : 0)),
			Patch(VK_VERSION_PATCH(version) >> (vendorId == 0x10DE ? 4 : 0))
		{
			// NVIDIA specific driver versioning.
			// https://github.com/SaschaWillems/VulkanCapsViewer/blob/master/vulkanDeviceInfo.cpp
			// 10 bits = major version (up to 1023)
			// 8 bits = minor version (up to 255)
			// 8 bits = secondary branch version/build version (up to 255)
			// 6 bits = tertiary branch/build version (up to 63)
		}

		const unsigned Major;
		const unsigned Minor;
		const unsigned Patch;

		friend std::ostream& operator << (std::ostream& out, const Version& version)
		{
			return out << version.Major << "." << version.Minor << "." << version.Patch;
		}
	};

}
