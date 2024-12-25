#pragma once

#include "Vulkan.hpp"
#include <map>
#include <vector>

namespace Vulkan
{
	class Buffer;
	class DescriptorPool;
	class DescriptorSetLayout;
	class ImageView;

	class DescriptorSets final
	{
	public:

		VULKAN_NON_COPIABLE(DescriptorSets)

		DescriptorSets(
			const DescriptorPool& descriptorPool, 
			const DescriptorSetLayout& layout,
		    std::map<uint32_t, VkDescriptorType> bindingTypes, 
			size_t size);

		~DescriptorSets();

		VkDescriptorSet Handle(size_t index) const { return descriptorSets_[index]; }

		VkWriteDescriptorSet Bind(size_t index, uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, uint32_t count = 1) const;
		VkWriteDescriptorSet Bind(size_t index, uint32_t binding, const VkDescriptorImageInfo& imageInfo, uint32_t count = 1) const;
		VkWriteDescriptorSet Bind(size_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, uint32_t count = 1) const;

		void UpdateDescriptors(const std::vector<VkWriteDescriptorSet>& descriptorWrites);

	private:

		VkDescriptorType GetBindingType(uint32_t binding) const;

		const DescriptorPool& descriptorPool_;
		const std::map<uint32_t, VkDescriptorType> bindingTypes_;
		
		std::vector<VkDescriptorSet> descriptorSets_;
	};

}