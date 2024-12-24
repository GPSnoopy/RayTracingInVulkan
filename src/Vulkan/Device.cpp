#include "Device.hpp"
#include "Enumerate.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "Utilities/Exception.hpp"
#include <algorithm>
#include <set>

namespace Vulkan {

namespace
{
	std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
		const std::vector<VkQueueFamilyProperties>& queueFamilies,
		const std::string& name,
		const VkQueueFlags requiredBits,
		const VkQueueFlags excludedBits)
	{
		const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
		{
			return 
				queueFamily.queueCount > 0 && 
				queueFamily.queueFlags & requiredBits &&
				!(queueFamily.queueFlags & excludedBits);
		});

		if (family == queueFamilies.end())
		{
			Throw(std::runtime_error("found no matching " + name + " queue"));
		}

		return family;
	}
}

Device::Device(
	VkPhysicalDevice physicalDevice, 
	const class Surface& surface, 
	const std::vector<const char*>& requiredExtensions,
	const VkPhysicalDeviceFeatures& deviceFeatures,
	const void* nextDeviceFeatures) :
	physicalDevice_(physicalDevice),
	surface_(surface),
	debugUtils_(surface.Instance().Handle())
{
	CheckRequiredExtensions(physicalDevice, requiredExtensions);

	const auto queueFamilies = GetEnumerateVector(physicalDevice, vkGetPhysicalDeviceQueueFamilyProperties);

	// Find the graphics queue.
	const auto graphicsFamily = FindQueue(queueFamilies, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
	const auto computeFamily = FindQueue(queueFamilies, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);

	//Commented out the dedicated transfer queue, as it's never used (relic from Vulkan tutorial) 
	//and causes problems with RADV (see https://github.com/NVIDIA/Q2RTX/issues/147).
	//const auto transferFamily = FindQueue(queueFamilies, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

	// Find the presentation queue (usually the same as graphics queue).
	const auto presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueFamily)
	{
		VkBool32 presentSupport = false;
		const uint32_t i = static_cast<uint32_t>(&*queueFamilies.cbegin() - &queueFamily);
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface.Handle(), &presentSupport);
		return queueFamily.queueCount > 0 && presentSupport;
	});

	if (presentFamily == queueFamilies.end())
	{
		Throw(std::runtime_error("found no presentation queue"));
	}

	graphicsFamilyIndex_ = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
	//computeFamilyIndex_ = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
	presentFamilyIndex_ = static_cast<uint32_t>(presentFamily - queueFamilies.begin());
	//transferFamilyIndex_ = static_cast<uint32_t>(transferFamily - queueFamilies.begin());

	// Queues can be the same
	const std::set<uint32_t> uniqueQueueFamilies =
	{
		graphicsFamilyIndex_,
		//computeFamilyIndex_,
		presentFamilyIndex_,
		//transferFamilyIndex_
	};

	// Create queues
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Create device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nextDeviceFeatures;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledLayerCount = static_cast<uint32_t>(surface_.Instance().ValidationLayers().size());
	createInfo.ppEnabledLayerNames = surface_.Instance().ValidationLayers().data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	Check(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_),
		"create logical device");

	debugUtils_.SetDevice(device_);

	vkGetDeviceQueue(device_, graphicsFamilyIndex_, 0, &graphicsQueue_);
	//vkGetDeviceQueue(device_, computeFamilyIndex_, 0, &computeQueue_);
	vkGetDeviceQueue(device_, presentFamilyIndex_, 0, &presentQueue_);
	//vkGetDeviceQueue(device_, transferFamilyIndex_, 0, &transferQueue_);
}

Device::~Device()
{
	if (device_ != nullptr)
	{
		vkDestroyDevice(device_, nullptr);
		device_ = nullptr;
	}
}

void Device::WaitIdle() const
{
	Check(vkDeviceWaitIdle(device_),
		"wait for device idle");
}

void Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const
{
	const auto availableExtensions = GetEnumerateVector(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
	std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& extension : availableExtensions) 
	{
		required.erase(extension.extensionName);
	}

	if (!required.empty())
	{
		bool first = true;
		std::string extensions;

		for (const auto& extension : required)
		{
			if (!first)
			{
				extensions += ", ";
			}

			extensions += extension;
			first = false;
		}

		Throw(std::runtime_error("missing required extensions: " + extensions));
	}
}

}
