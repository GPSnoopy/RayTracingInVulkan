
#include "Vulkan/Enumerate.hpp"
#include "Vulkan/Strings.hpp"
#include "Vulkan/Version.hpp"
#include "Utilities/Console.hpp"
#include "Utilities/Exception.hpp"
#include "Options.hpp"
#include "RayTracer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace
{
	UserSettings CreateUserSettings(const Options& options);
	void PrintVulkanDevices(const Vulkan::Application& application);
	void PrintVulkanExtensions(const Vulkan::Application& application, bool benchmark);
	void SetVulkanDevice(Vulkan::Application& application);
}

int main(int argc, const char* argv[]) noexcept
{
	try
	{
		const Options options(argc, argv);
		const UserSettings userSettings = CreateUserSettings(options);
		const Vulkan::WindowConfig windowConfig{ "Vulkan Window", options.Width, options.Height, options.Fullscreen, !options.Fullscreen };

		RayTracer application(userSettings, windowConfig, options.VSync);

		PrintVulkanDevices(application);
		PrintVulkanExtensions(application, options.Benchmark);
		SetVulkanDevice(application);

		application.Run();

		return EXIT_SUCCESS;
	}

	catch (const Options::Help&)
	{
		return EXIT_SUCCESS;
	}

	catch (const std::exception& exception)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal, [&exception]() 
		{
			const auto stacktrace = boost::get_error_info<traced>(exception);

			std::cerr << "FATAL: " << exception.what() << std::endl;

			if (stacktrace)
			{
				std::cerr << '\n' << *stacktrace << '\n';
			}
		});
	}

	catch (...)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal, []()
		{
			std::cerr << "FATAL: caught unhandled exception" << std::endl;
		});
	}

	return EXIT_FAILURE;
}

namespace
{

	UserSettings CreateUserSettings(const Options& options)
	{
		UserSettings userSettings{};

		userSettings.Benchmark = options.Benchmark;
		userSettings.SceneIndex = options.SceneIndex;
		userSettings.IsRayTraced = true;
		userSettings.AccumulateRays = true;
		userSettings.NumberOfSamples = options.Samples;
		userSettings.NumberOfBounces = options.Bounces;
		userSettings.ShowSettings = !options.Benchmark;
		userSettings.ShowOverlay = true;

		return userSettings;
	}

	void PrintVulkanDevices(const Vulkan::Application& application)
	{
		std::cout << "Vulkan Devices: " << std::endl;

		for (const auto& device : application.PhysicalDevices())
		{
			VkPhysicalDeviceProperties prop;
			vkGetPhysicalDeviceProperties(device, &prop);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			const Vulkan::Version vulkanVersion(prop.apiVersion);
			const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

			std::cout << "- [" << prop.deviceID << "] ";
			std::cout << Vulkan::Strings::VendorId(prop.vendorID) << " '" << prop.deviceName;
			std::cout << "' (";
			std::cout << Vulkan::Strings::DeviceType(prop.deviceType) << ": ";
			std::cout << "vulkan " << vulkanVersion << ", ";
			std::cout << "driver " << driverVersion;
			std::cout << ")" << std::endl;
		}
	}

	void PrintVulkanExtensions(const Vulkan::Application& application, const bool benchmark)
	{
		if (benchmark)
		{
			return;
		}

		std::cout << "Vulkan Instance Extensions: " << std::endl;

		for (const auto& extension : application.Extensions())
		{
			std::cout << "- " << extension.extensionName << " (" << Vulkan::Version(extension.specVersion) << ")" << std::endl;
		}
	}

	void SetVulkanDevice(Vulkan::Application& application)
	{
		const auto& physicalDevices = application.PhysicalDevices();
		const auto result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice& device)
		{
			// We want a device with geometry shader support.
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			if (!deviceFeatures.geometryShader)
			{
				return false;
			}

			// We want a device with a graphics queue.
			const auto queueFamilies = Vulkan::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties);
			const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
			{
				return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
			});

			return hasGraphicsQueue != queueFamilies.end();
		});

		if (result == physicalDevices.end())
		{
			Throw(std::runtime_error("cannot find a suitable device"));
		}

		application.SetPhysicalDevice(*result);
	}

}
