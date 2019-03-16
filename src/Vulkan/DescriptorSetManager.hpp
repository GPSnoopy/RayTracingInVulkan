#pragma once

#include "DescriptorBinding.hpp"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Device;
	class DescriptorPool;
	class DescriptorSetLayout;
	class DescriptorSets;

	class DescriptorSetManager final
	{
	public:

		VULKAN_NON_COPIABLE(DescriptorSetManager)

		explicit DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
		~DescriptorSetManager();

		const class DescriptorSetLayout& DescriptorSetLayout() const { return *descriptorSetLayout_; }
		class DescriptorSets& DescriptorSets() { return *descriptorSets_; }

	private:

		std::unique_ptr<DescriptorPool> descriptorPool_;
		std::unique_ptr<class DescriptorSetLayout> descriptorSetLayout_;
		std::unique_ptr<class DescriptorSets> descriptorSets_;
	};

}
