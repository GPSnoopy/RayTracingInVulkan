#pragma once

#include "Vulkan.hpp"
#include <memory>

namespace Vulkan
{
	class CommandPool;
	class Device;
	class DeviceMemory;
	class Image;
	class ImageView;

	class DepthBuffer final
	{
	public:

		VULKAN_NON_COPIABLE(DepthBuffer)

		DepthBuffer(CommandPool& commandPool, VkExtent2D extent);
		~DepthBuffer();

		VkFormat Format() const { return format_; }
		const class ImageView& ImageView() const { return *imageView_; }

		static bool HasStencilComponent(const VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

	private:

		const VkFormat format_;
		std::unique_ptr<Image> image_;
		std::unique_ptr<DeviceMemory> imageMemory_;
		std::unique_ptr<class ImageView> imageView_;
	};

}
