#include "DebugUtilsMessenger.hpp"
#include "Instance.hpp"
#include "Utilities/Console.hpp"
#include "Utilities/Exception.hpp"
#include <iostream>

namespace Vulkan {

	namespace
	{
		const char* ObjectTypeToString(const VkObjectType objectType)
		{
			switch (objectType)
			{
			case VK_OBJECT_TYPE_UNKNOWN:                        return "unknown";
			case VK_OBJECT_TYPE_INSTANCE:                       return "instance";
			case VK_OBJECT_TYPE_PHYSICAL_DEVICE:                return "physical device";
			case VK_OBJECT_TYPE_DEVICE:                         return "device";
			case VK_OBJECT_TYPE_QUEUE:                          return "queue";
			case VK_OBJECT_TYPE_SEMAPHORE:                      return "semaphore";
			case VK_OBJECT_TYPE_COMMAND_BUFFER:                 return "cmd buffer";
			case VK_OBJECT_TYPE_FENCE:                          return "fence";
			case VK_OBJECT_TYPE_DEVICE_MEMORY:                  return "memory";
			case VK_OBJECT_TYPE_BUFFER:                         return "buffer";
			case VK_OBJECT_TYPE_IMAGE:                          return "image";
			case VK_OBJECT_TYPE_EVENT:                          return "event";
			case VK_OBJECT_TYPE_QUERY_POOL:                     return "query pool";
			case VK_OBJECT_TYPE_BUFFER_VIEW:                    return "buffer view";
			case VK_OBJECT_TYPE_IMAGE_VIEW:                     return "image view";
			case VK_OBJECT_TYPE_SHADER_MODULE:                  return "shader module";
			case VK_OBJECT_TYPE_PIPELINE_CACHE:                 return "pipeline cache";
			case VK_OBJECT_TYPE_PIPELINE_LAYOUT:                return "pipeline layout";
			case VK_OBJECT_TYPE_RENDER_PASS:                    return "render pass";
			case VK_OBJECT_TYPE_PIPELINE:                       return "pipeline";
			case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:          return "descriptor set layout";
			case VK_OBJECT_TYPE_SAMPLER:                        return "sampler";
			case VK_OBJECT_TYPE_DESCRIPTOR_POOL:                return "descriptor pool";
			case VK_OBJECT_TYPE_DESCRIPTOR_SET:                 return "descriptor set";
			case VK_OBJECT_TYPE_FRAMEBUFFER:                    return "framebuffer";
			case VK_OBJECT_TYPE_COMMAND_POOL:                   return "command pool";
			case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:       return "sampler ycbcr conversion";
			case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:     return "descriptor update template";
			case VK_OBJECT_TYPE_SURFACE_KHR:                    return "surface KHR";
			case VK_OBJECT_TYPE_SWAPCHAIN_KHR:                  return "swapchain KHR";
			case VK_OBJECT_TYPE_DISPLAY_KHR:                    return "display KHR";
			case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:               return "display mode KHR";
			case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:      return "debug report callback";
			case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:               return "object tabke NVX";
			case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:   return "indirect cmd layout NVX";
			case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:      return "debug utils messenger";
			case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:           return "validation cache";
			case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV:      return "acceleration structure NV";
			default: return "unknown";
			}
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
			void* const pUserData)
		{
			(void)pUserData;

			const auto attributes = Utilities::Console::SetColorBySeverity(static_cast<Utilities::Severity>(messageSeverity));

			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				std::cerr << "VERBOSE: ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				std::cerr << "INFO: ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				std::cerr << "WARNING: ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				std::cerr << "ERROR: ";
				break;
			default:;
				std::cerr << "UNKNOWN: ";
			}

			switch (messageType)
			{
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				std::cerr << "GENERAL: ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				std::cerr << "VALIDATION: ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				std::cerr << "PERFORMANCE: ";
				break;
			default:
				std::cerr << "UNKNOWN: ";
			}

			std::cerr << pCallbackData->pMessage;

			if (pCallbackData->objectCount > 0 && messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				std::cerr << "\n\n  Objects (" << pCallbackData->objectCount << "):\n";

				for (uint32_t i = 0; i != pCallbackData->objectCount; ++i)
				{
					const auto object = pCallbackData->pObjects[i];
					std::cerr
						<< "  - Object[" << i << "]: "
						<< "Type: " << ObjectTypeToString(object.objectType ) << ", "
						<< "Handle: " << reinterpret_cast<void*>(object.objectHandle) << ", "
						<< "Name: '" << (object.pObjectName ? object.pObjectName : "") << "'"
						<< "\n";
				}
			}

			std::cerr << std::endl;

			Utilities::Console::SetColorByAttributes(attributes);

			return VK_FALSE;
		}

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
		{
			const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
			return func != nullptr
				? func(instance, pCreateInfo, pAllocator, pCallback)
				: VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
		{
			const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
			if (func != nullptr) {
				func(instance, callback, pAllocator);
			}
		}
	}

DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold) :
	instance_(instance),
	threshold_(threshold)
{
	if (instance.ValidationLayers().empty())
	{
		return;
	}

	VkDebugUtilsMessageSeverityFlagsEXT severity = 0;

	switch (threshold)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: 
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		break;
	default:
		Throw(std::invalid_argument("invalid threshold"));
	}

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = severity;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = nullptr;

	Check(CreateDebugUtilsMessengerEXT(instance_.Handle(), &createInfo, nullptr, &messenger_),
		"set up Vulkan debug callback");
}

DebugUtilsMessenger::~DebugUtilsMessenger()
{
	if (messenger_ != nullptr)
	{
		DestroyDebugUtilsMessengerEXT(instance_.Handle(), messenger_, nullptr);
		messenger_ = nullptr;
	}
}

}
