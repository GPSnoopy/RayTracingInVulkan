#include "Image.hpp"
#include "Buffer.hpp"
#include "DepthBuffer.hpp"
#include "Device.hpp"
#include "SingleTimeCommands.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Image::Image(const class Device& device, const VkExtent2D extent, const VkFormat format) :
	Image(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
{
}

Image::Image(
	const class Device& device, 
	const VkExtent2D extent,
	const VkFormat format,
	const VkImageTiling tiling,
	const VkImageUsageFlags usage) :
	device_(device),
	extent_(extent),
	format_(format),
	imageLayout_(VK_IMAGE_LAYOUT_UNDEFINED)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = extent.width;
	imageInfo.extent.height = extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = imageLayout_;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	Check(vkCreateImage(device.Handle(), &imageInfo, nullptr, &image_),
		"create image");
}

Image::Image(Image&& other) noexcept :
	device_(other.device_),
	extent_(other.extent_),
	format_(other.format_),
	imageLayout_(other.imageLayout_),
	image_(other.image_)
{
	other.image_ = nullptr;
}

Image::~Image()
{
	if (image_ != nullptr)
	{
		vkDestroyImage(device_.Handle(), image_, nullptr);
		image_ = nullptr;
	}
}

DeviceMemory Image::AllocateMemory(const VkMemoryPropertyFlags properties) const
{
	const auto requirements = GetMemoryRequirements();
	DeviceMemory memory(device_, requirements.size, requirements.memoryTypeBits, 0, properties);

	Check(vkBindImageMemory(device_.Handle(), image_, memory.Handle(), 0),
		"bind image memory");

	return memory;
}

VkMemoryRequirements Image::GetMemoryRequirements() const
{
	VkMemoryRequirements requirements;
	vkGetImageMemoryRequirements(device_.Handle(), image_, &requirements);
	return requirements;
}

void Image::TransitionImageLayout(CommandPool& commandPool, VkImageLayout newLayout)
{
	SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = imageLayout_;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image_;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (DepthBuffer::HasStencilComponent(format_)) 
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else 
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (imageLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (imageLayout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (imageLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else 
		{
			Throw(std::invalid_argument("unsupported layout transition"));
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	});

	imageLayout_ = newLayout;
}

void Image::CopyFrom(CommandPool& commandPool, const Buffer& buffer)
{
	SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { extent_.width, extent_.height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer.Handle(), image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	});
}

}
