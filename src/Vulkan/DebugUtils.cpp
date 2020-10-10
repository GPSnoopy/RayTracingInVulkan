#include "DebugUtils.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {
	
DebugUtils::DebugUtils(VkInstance instance)
	: vkSetDebugUtilsObjectNameEXT_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")))
{
#ifndef NDEBUG
	if (vkSetDebugUtilsObjectNameEXT_ == nullptr)
	{
		Throw(std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'"));
	}
#endif
}

}