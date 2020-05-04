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
#define STR(e) case VK_OBJECT_TYPE_ ## e: return # e
			STR(UNKNOWN);
			STR(INSTANCE);
			STR(PHYSICAL_DEVICE);
			STR(DEVICE);
			STR(QUEUE);
			STR(SEMAPHORE);
			STR(COMMAND_BUFFER);
			STR(FENCE);
			STR(DEVICE_MEMORY);
			STR(BUFFER);
			STR(IMAGE);
			STR(EVENT);
			STR(QUERY_POOL);
			STR(BUFFER_VIEW);
			STR(IMAGE_VIEW);
			STR(SHADER_MODULE);
			STR(PIPELINE_CACHE);
			STR(PIPELINE_LAYOUT);
			STR(RENDER_PASS);
			STR(PIPELINE);
			STR(DESCRIPTOR_SET_LAYOUT);
			STR(SAMPLER);
			STR(DESCRIPTOR_POOL);
			STR(DESCRIPTOR_SET);
			STR(FRAMEBUFFER);
			STR(COMMAND_POOL);
			STR(SAMPLER_YCBCR_CONVERSION);
			STR(DESCRIPTOR_UPDATE_TEMPLATE);
			STR(SURFACE_KHR);
			STR(SWAPCHAIN_KHR);
			STR(DISPLAY_KHR);
			STR(DISPLAY_MODE_KHR);
			STR(DEBUG_REPORT_CALLBACK_EXT);
			STR(DEBUG_UTILS_MESSENGER_EXT);
			STR(ACCELERATION_STRUCTURE_KHR);
			STR(VALIDATION_CACHE_EXT);
			STR(PERFORMANCE_CONFIGURATION_INTEL);
			STR(DEFERRED_OPERATION_KHR);
			STR(INDIRECT_COMMANDS_LAYOUT_NV);
#undef STR
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
