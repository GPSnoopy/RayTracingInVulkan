#include "TextureImage.hpp"
#include "Texture.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/Sampler.hpp"
#include <cstring>

namespace Assets {

TextureImage::TextureImage(Vulkan::CommandPool& commandPool, const Texture& texture)
{
	// Create a host staging buffer and copy the image into it.
	const VkDeviceSize imageSize = texture.Width() * texture.Height() * 4;
	const auto& device = commandPool.Device();

	auto stagingBuffer = std::make_unique<Vulkan::Buffer>(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	const auto data = stagingBufferMemory.Map(0, imageSize);
	std::memcpy(data, texture.Pixels(), imageSize);
	stagingBufferMemory.Unmap();

	// Create the device side image, memory, view and sampler.
	image_.reset(new Vulkan::Image(device, VkExtent2D{ static_cast<uint32_t>(texture.Width()), static_cast<uint32_t>(texture.Height()) }, VK_FORMAT_R8G8B8A8_UNORM));
	imageMemory_.reset(new Vulkan::DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	imageView_.reset(new Vulkan::ImageView(device, image_->Handle(), image_->Format(), VK_IMAGE_ASPECT_COLOR_BIT));
	sampler_.reset(new Vulkan::Sampler(device, Vulkan::SamplerConfig()));

	// Transfer the data to device side.
	image_->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	image_->CopyFrom(commandPool, *stagingBuffer);
	image_->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// Delete the buffer before the memory
	stagingBuffer.reset();
}

TextureImage::~TextureImage()
{
	sampler_.reset();
	imageView_.reset();
	image_.reset();
	imageMemory_.reset();
}

}
