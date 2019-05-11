#include "UniformBuffer.hpp"
#include "Vulkan/Buffer.hpp"
#include <cstring>

namespace Assets {

UniformBuffer::UniformBuffer(const Vulkan::Device& device)
{
	const auto bufferSize = sizeof(UniformBufferObject);

	buffer_.reset(new Vulkan::Buffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
	memory_.reset(new Vulkan::DeviceMemory(buffer_->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept :
	buffer_(other.buffer_.release()),
	memory_(other.memory_.release())
{
}

UniformBuffer::~UniformBuffer()
{
	buffer_.reset();
	memory_.reset(); // release memory after bound buffer has been destroyed
}

void UniformBuffer::SetValue(const UniformBufferObject& ubo)
{
	const auto data = memory_->Map(0, sizeof(UniformBufferObject));
	std::memcpy(data, &ubo, sizeof(ubo));
	memory_->Unmap();
}

}
