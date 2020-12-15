#include "Buffer.hpp"
#include "SingleTimeCommands.hpp"

namespace Vulkan {

Buffer::Buffer(const class Device& device, const size_t size, const VkBufferUsageFlags usage) :
	device_(device)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Check(vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, &buffer_),
		"create buffer");
}

Buffer::~Buffer()
{
	if (buffer_ != nullptr)
	{
		vkDestroyBuffer(device_.Handle(), buffer_, nullptr);
		buffer_ = nullptr;
	}
}

DeviceMemory Buffer::AllocateMemory(const VkMemoryPropertyFlags propertyFlags)
{
	return AllocateMemory(0, propertyFlags);
}

DeviceMemory Buffer::AllocateMemory(const VkMemoryAllocateFlags allocateFlags, const VkMemoryPropertyFlags propertyFlags)
{
	const auto requirements = GetMemoryRequirements();
	DeviceMemory memory(device_, requirements.size, requirements.memoryTypeBits, allocateFlags, propertyFlags);

	Check(vkBindBufferMemory(device_.Handle(), buffer_, memory.Handle(), 0),
		"bind buffer memory");

	return memory;
}

VkMemoryRequirements Buffer::GetMemoryRequirements() const
{
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(device_.Handle(), buffer_, &requirements);
	return requirements;
}

VkDeviceAddress Buffer::GetDeviceAddress() const
{
	VkBufferDeviceAddressInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	info.pNext = nullptr;
	info.buffer = Handle();

	return vkGetBufferDeviceAddress(device_.Handle(), &info);
}

void Buffer::CopyFrom(CommandPool& commandPool, const Buffer& src, VkDeviceSize size)
{
	SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, src.Handle(), Handle(), 1, &copyRegion);
	});
}

}
