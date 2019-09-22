#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class Device;

	struct SamplerConfig final
	{
		VkFilter MagFilter = VK_FILTER_LINEAR;
		VkFilter MinFilter = VK_FILTER_LINEAR;
		VkSamplerAddressMode AddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		VkSamplerAddressMode AddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		VkSamplerAddressMode AddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		bool AnisotropyEnable = true;
		float MaxAnisotropy = 16;
		VkBorderColor BorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		bool UnnormalizedCoordinates = false;
		bool CompareEnable = false;
		VkCompareOp CompareOp = VK_COMPARE_OP_ALWAYS;
		VkSamplerMipmapMode MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		float MipLodBias = 0.0f;
		float MinLod = 0.0f;
		float MaxLod = 0.0f;
	};

	class Sampler final
	{
	public:

		VULKAN_NON_COPIABLE(Sampler)

		Sampler(const Device& device, const SamplerConfig& config);
		~Sampler();

		const class Device& Device() const { return device_; }

	private:

		const class Device& device_;

		VULKAN_HANDLE(VkSampler, sampler_)
	};

}
