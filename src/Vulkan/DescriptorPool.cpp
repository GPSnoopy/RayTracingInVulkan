#include "DescriptorPool.hpp"
#include "Device.hpp"

namespace Vulkan {

DescriptorPool::DescriptorPool(const Vulkan::Device& device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets) :
	device_(device)
{
	std::vector<VkDescriptorPoolSize> poolSizes;

	for (const auto& binding : descriptorBindings)
	{
		poolSizes.push_back(VkDescriptorPoolSize{ binding.Type, static_cast<uint32_t>(binding.DescriptorCount*maxSets )});
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxSets);

	Check(vkCreateDescriptorPool(device.Handle(), &poolInfo, nullptr, &descriptorPool_),
		"create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
	if (descriptorPool_ != nullptr)
	{
		vkDestroyDescriptorPool(device_.Handle(), descriptorPool_, nullptr);
		descriptorPool_ = nullptr;
	}
}

}
