#include "SwapChain.hpp"
#include "Device.hpp"
#include "Enumerate.hpp"
#include "ImageView.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "Window.hpp"
#include "Utilities/Exception.hpp"
#include <algorithm>
#include <limits>

namespace Vulkan {

SwapChain::SwapChain(const class Device& device, const bool vsync) :
	physicalDevice_(device.PhysicalDevice()),
	device_(device)
{
	const auto details = QuerySwapChainSupport(device.PhysicalDevice(), device.Surface().Handle());
	if (details.Formats.empty() || details.PresentModes.empty())
	{
		throw std::runtime_error("empty swap chain support");
	}

	const auto& surface = device.Surface();
	const auto& window = surface.Instance().Window();

	const auto surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
	const auto presentMode = ChooseSwapPresentMode(details.PresentModes, vsync);
	const auto extent = ChooseSwapExtent(window, details.Capabilities);
	const auto imageCount = ChooseImageCount(details.Capabilities);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.Handle();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	createInfo.preTransform = details.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = nullptr;
	
	if (device.GraphicsFamilyIndex() != device.PresentFamilyIndex())
	{
		uint32_t queueFamilyIndices[] = { device.GraphicsFamilyIndex(), device.PresentFamilyIndex() };

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	Check(vkCreateSwapchainKHR(device.Handle(), &createInfo, nullptr, &swapChain_),
		"create swap chain!");

	minImageCount_ = details.Capabilities.minImageCount;
	format_ = surfaceFormat.format;
	extent_ = extent;
	images_ = GetEnumerateVector(device_.Handle(), swapChain_, vkGetSwapchainImagesKHR);
	imageViews_.reserve(images_.size());

	for (const auto image : images_)
	{
		imageViews_.push_back(std::make_unique<ImageView>(device, image, format_, VK_IMAGE_ASPECT_COLOR_BIT));
	}
}

SwapChain::~SwapChain()
{
	imageViews_.clear();

	if (swapChain_ != nullptr)
	{
		vkDestroySwapchainKHR(device_.Handle(), swapChain_, nullptr);
		swapChain_ = nullptr;
	}
}

SwapChain::SupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
{
	SupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);
	details.Formats = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfaceFormatsKHR);
	details.PresentModes = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfacePresentModesKHR);

	return details;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) 
{
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) 
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	Throw(std::runtime_error("found no suitable surface format"));
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, const bool vsync) 
{
	// VK_PRESENT_MODE_IMMEDIATE_KHR: 
	//   Images submitted by your application are transferred to the screen right away, which may result in tearing.
	// VK_PRESENT_MODE_FIFO_KHR: 
	//   The swap chain is a queue where the display takes an image from the front of the queue when the display is
	//   refreshed and the program inserts rendered images at the back of the queue. If the queue is full then the program 
	//   has to wait. This is most similar to vertical sync as found in modern games. The moment that the display is 
	//   refreshed is known as "vertical blank".
	// VK_PRESENT_MODE_FIFO_RELAXED_KHR:
	//   This mode only differs from the previous one if the application is late and the queue was empty at the last 
	//   vertical blank. Instead of waiting for the next vertical blank, the image is transferred right away when it 
	//   finally arrives. This may result in visible tearing.
	// VK_PRESENT_MODE_MAILBOX_KHR: 
	//   This is another variation of the second mode. Instead of blocking the application when the queue is full, the 
	//   images that are already queued are simply replaced with the newer ones.This mode can be used to implement triple 
	//   buffering, which allows you to avoid tearing with significantly less latency issues than standard vertical sync 
	//   that uses double buffering.

	if (vsync)
	{
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end())
	{
		return VK_PRESENT_MODE_MAILBOX_KHR;
	}

	if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != presentModes.end())
	{
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}

	if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_FIFO_RELAXED_KHR) != presentModes.end())
	{
		return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
	// However, some window managers do allow us to differ here and this is indicated by setting the width and height in
	// currentExtent to a special value: the maximum value of uint32_t. In that case we'll pick the resolution that best 
	// matches the window within the minImageExtent and maxImageExtent bounds.
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	auto actualExtent = window.FramebufferSize();

	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

uint32_t SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// The implementation specifies the minimum amount of images to function properly
	// and we'll try to have one more than that to properly implement triple buffering.
	// (tanguyf: or not, we can just rely on VK_PRESENT_MODE_MAILBOX_KHR with two buffers)
	uint32_t imageCount = capabilities.minImageCount;// +1; 
	
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	return imageCount;
}

}
