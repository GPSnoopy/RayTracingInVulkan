#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	struct DescriptorBinding
	{
		uint32_t Binding; // Slot to which the descriptor will be bound, corresponding to the layout index in the shader.
		uint32_t DescriptorCount; // Number of descriptors to bind
		VkDescriptorType Type; // Type of the bound descriptor(s)
		VkShaderStageFlags Stage; // Shader stage at which the bound resources will be available
	};
}