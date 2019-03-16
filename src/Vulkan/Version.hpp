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

		const unsigned Major;
		const unsigned Minor;
		const unsigned Patch;

		friend std::ostream& operator << (std::ostream& out, const Version& version)
		{
			return out << version.Major << "." << version.Minor << "." << version.Patch;
		}
	};

}
