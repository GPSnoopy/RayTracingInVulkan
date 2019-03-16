#pragma once

#include "DescriptorBinding.hpp"
#include <vector>

namespace Vulkan
{
	class Device;

	class DescriptorPool final
	{
	public:

		VULKAN_NON_COPIABLE(DescriptorPool)

		DescriptorPool(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
		~DescriptorPool();

		const class Device& Device() const { return device_; }

	private:

		const class Device& device_;

		VULKAN_HANDLE(VkDescriptorPool, descriptorPool_)
	};

}
