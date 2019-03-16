#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class DescriptorSetLayout;
	class Device;

	class PipelineLayout final
	{
	public:

		VULKAN_NON_COPIABLE(PipelineLayout)

		PipelineLayout(const Device& device, const DescriptorSetLayout& descriptorSetLayout);
		~PipelineLayout();

	private:

		const Device& device_;

		VULKAN_HANDLE(VkPipelineLayout, pipelineLayout_)
	};

}
