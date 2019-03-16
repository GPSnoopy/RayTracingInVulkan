#pragma once

#include "Vulkan.hpp"
#include <vector>

namespace Vulkan
{
	class Window;

	class Instance final
	{
	public:

		VULKAN_NON_COPIABLE(Instance)

		Instance(const Window& window, const std::vector<const char*>& validationLayers);
		~Instance();

		const class Window& Window() const { return window_; }

		const std::vector<VkExtensionProperties>& Extensions() const { return extensions_; }
		const std::vector<VkPhysicalDevice>& PhysicalDevices() const { return physicalDevices_; }
		const std::vector<const char*>& ValidationLayers() const { return validationLayers_; }

	private:

		void GetVulkanDevices();
		void GetVulkanExtensions();

		static void CheckVulkanMinimumVersion(uint32_t minVersion);
		static void CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers);

		const class Window& window_;
		const std::vector<const char*> validationLayers_;

		VULKAN_HANDLE(VkInstance, instance_)

		std::vector<VkPhysicalDevice> physicalDevices_;
		std::vector<VkExtensionProperties> extensions_;
	};

}
