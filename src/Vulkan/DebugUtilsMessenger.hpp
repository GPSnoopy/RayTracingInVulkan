#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class Instance;

	class DebugUtilsMessenger final
	{
	public:

		VULKAN_NON_COPIABLE(DebugUtilsMessenger)

		DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
		~DebugUtilsMessenger();

		VkDebugUtilsMessageSeverityFlagBitsEXT Threshold() const { return threshold_; }

	private:

		const Instance& instance_;
		const VkDebugUtilsMessageSeverityFlagBitsEXT threshold_;

		VULKAN_HANDLE(VkDebugUtilsMessengerEXT, messenger_)
	};

}
