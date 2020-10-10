#include "Instance.hpp"
#include "Enumerate.hpp"
#include "Version.hpp"
#include "Window.hpp"
#include "Utilities/Exception.hpp"
#include <algorithm>
#include <sstream>

namespace Vulkan {

Instance::Instance(const class Window& window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion) :
	window_(window),
	validationLayers_(validationLayers)
{
	// Check the minimum version.
	CheckVulkanMinimumVersion(vulkanVersion);

	// Get the list of required extensions.
	auto extensions = window.GetRequiredInstanceExtensions();

	// Check the validation layers and add them to the list of required extensions.
	CheckVulkanValidationLayerSupport(validationLayers);

	if (!validationLayers.empty())
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Create the Vulkan instance.
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "RayTracingWeekends";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = vulkanVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	Check(vkCreateInstance(&createInfo, nullptr, &instance_),
		"create instance");

	GetVulkanPhysicalDevices();
	GetVulkanLayers();
	GetVulkanExtensions();
}

Instance::~Instance()
{
	if (instance_ != nullptr)
	{
		vkDestroyInstance(instance_, nullptr);
		instance_ = nullptr;
	}
}

void Instance::GetVulkanExtensions()
{
	GetEnumerateVector(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, extensions_);
}

void Instance::GetVulkanLayers()
{
	GetEnumerateVector(vkEnumerateInstanceLayerProperties, layers_);
}

void Instance::GetVulkanPhysicalDevices()
{
	GetEnumerateVector(instance_, vkEnumeratePhysicalDevices, physicalDevices_);

	if (physicalDevices_.empty())
	{
		Throw(std::runtime_error("found no Vulkan physical devices"));
	}
}

void Instance::CheckVulkanMinimumVersion(const uint32_t minVersion)
{
	uint32_t version;
	Check(vkEnumerateInstanceVersion(&version),
		"query instance version");

	if (minVersion > version)
	{
		std::ostringstream out;
		out << "minimum required version not found (required " << Version(minVersion);
		out << ", found " << Version(version) << ")";

		Throw(std::runtime_error(out.str()));
	}
}

void Instance::CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
	const auto availableLayers = GetEnumerateVector(vkEnumerateInstanceLayerProperties);

	for (const char* layer : validationLayers)
	{
		auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties& layerProperties)
		{
			return strcmp(layer, layerProperties.layerName) == 0;
		});

		if (result == availableLayers.end())
		{
			Throw(std::runtime_error("could not find the requested validation layer: '" + std::string(layer) + "'"));
		}
	}
}

}
