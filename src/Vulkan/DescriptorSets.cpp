#include "DescriptorSets.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "Utilities/Exception.hpp"
#include <array>
#include <utility>

namespace Vulkan {

DescriptorSets::DescriptorSets(
	const DescriptorPool& descriptorPool, 
	const DescriptorSetLayout& layout,
	std::map<uint32_t, VkDescriptorType> bindingTypes,
	const size_t size) :
	descriptorPool_(descriptorPool),
	bindingTypes_(std::move(bindingTypes))
{
	std::vector<VkDescriptorSetLayout> layouts(size, layout.Handle());

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool.Handle();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets_.resize(size);

	Check(vkAllocateDescriptorSets(descriptorPool.Device().Handle(), &allocInfo, descriptorSets_.data()),
		"allocate descriptor sets");
}

DescriptorSets::~DescriptorSets()
{
	//if (!descriptorSets_.empty())
	//{
	//	vkFreeDescriptorSets(
	//		descriptorPool_.Device().Handle(),
	//		descriptorPool_.Handle(),
	//		static_cast<uint32_t>(descriptorSets_.size()),
	//		descriptorSets_.data());

	//	descriptorSets_.clear();
	//}
}

VkWriteDescriptorSet DescriptorSets::Bind(const size_t index, const uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets_[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = GetBindingType(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(const size_t index, const uint32_t binding, const VkDescriptorImageInfo& imageInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets_[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = GetBindingType(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pImageInfo = &imageInfo;

	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(const size_t index, const uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets_[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = GetBindingType(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pNext = &structureInfo;

	return descriptorWrite;
}

void DescriptorSets::UpdateDescriptors(const std::vector<VkWriteDescriptorSet>& descriptorWrites)
{
	vkUpdateDescriptorSets(
		descriptorPool_.Device().Handle(),
		static_cast<uint32_t>(descriptorWrites.size()),
		descriptorWrites.data(), 0, nullptr);
}

VkDescriptorType DescriptorSets::GetBindingType(const uint32_t binding) const
{
	const auto it = bindingTypes_.find(binding);
	if (it == bindingTypes_.end())
	{
		Throw(std::invalid_argument("binding not found"));
	}

	return it->second;
}

}
