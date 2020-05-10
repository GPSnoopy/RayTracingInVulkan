#pragma once

#include "Vulkan.hpp"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Device;
	class ImageView;
	class Window;

	class SwapChain final
	{
	public:

		VULKAN_NON_COPIABLE(SwapChain)

		SwapChain(const Device& device, VkPresentModeKHR presentMode);
		~SwapChain();

		VkPhysicalDevice PhysicalDevice() const { return physicalDevice_; }
		const class Device& Device() const { return device_; }
		uint32_t MinImageCount() const { return minImageCount_; }
		const std::vector<VkImage>& Images() const { return images_; }
		const std::vector<std::unique_ptr<ImageView>>& ImageViews() const { return imageViews_; }
		const VkExtent2D& Extent() const { return extent_; }
		VkFormat Format() const { return format_; }
		VkPresentModeKHR PresentMode() const { return presentMode_; }

	private:

		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities{};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		static SupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode);
		static VkExtent2D ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
		static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

		const VkPhysicalDevice physicalDevice_;
		const class Device& device_;

		VULKAN_HANDLE(VkSwapchainKHR, swapChain_)

		uint32_t minImageCount_;
		VkPresentModeKHR presentMode_;
		VkFormat format_;
		VkExtent2D extent_{};
		std::vector<VkImage> images_;
		std::vector<std::unique_ptr<ImageView>> imageViews_;
	};

}
