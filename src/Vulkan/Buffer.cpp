#include "Buffer.hpp"
#include "SingleTimeCommands.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Buffer::Buffer(const class Device& device, const size_t size, const VkBufferUsageFlags usage) :
	device_(device)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, &buffer_) != VK_SUCCESS) 
	{
		Throw(std::runtime_error("failed to create buffer"));
	}
}

Buffer::~Buffer()
{
	if (buffer_ != nullptr)
	{
		vkDestroyBuffer(device_.Handle(), buffer_, nullptr);
		buffer_ = nullptr;
	}
}

DeviceMemory Buffer::AllocateMemory(const VkMemoryPropertyFlags properties) const
{
	const auto requirements = GetMemoryRequirements();
	DeviceMemory memory(device_, requirements.size, requirements.memoryTypeBits, properties);

	if (vkBindBufferMemory(device_.Handle(), buffer_, memory.Handle(), 0) != VK_SUCCESS)
	{
		Throw(std::runtime_error("failed to bind buffer memory"));
	}

	return memory;
}

VkMemoryRequirements Buffer::GetMemoryRequirements() const
{
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(device_.Handle(), buffer_, &requirements);
	return requirements;
}

void Buffer::CopyFrom(CommandPool& commandPool, const Buffer& src, VkDeviceSize size)
{
	SingleTimeCommands::Submit(commandPool, [&] (VkCommandBuffer commandBuffer)
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, src.Handle(), Handle(), 1, &copyRegion);
	});
}

}
