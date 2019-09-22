#include "Sampler.hpp"
#include "Device.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Sampler::Sampler(const class Device& device, const SamplerConfig& config) :
	device_(device)
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = config.MagFilter;
	samplerInfo.minFilter = config.MinFilter;
	samplerInfo.addressModeU = config.AddressModeU;
	samplerInfo.addressModeV = config.AddressModeV;
	samplerInfo.addressModeW = config.AddressModeW;
	samplerInfo.anisotropyEnable = config.AnisotropyEnable;
	samplerInfo.maxAnisotropy = config.MaxAnisotropy;
	samplerInfo.borderColor = config.BorderColor;
	samplerInfo.unnormalizedCoordinates = config.UnnormalizedCoordinates;
	samplerInfo.compareEnable = config.CompareEnable;
	samplerInfo.compareOp = config.CompareOp;
	samplerInfo.mipmapMode = config.MipmapMode;
	samplerInfo.mipLodBias = config.MipLodBias;
	samplerInfo.minLod = config.MinLod;
	samplerInfo.maxLod = config.MaxLod;

	if (vkCreateSampler(device.Handle(), &samplerInfo, nullptr, &sampler_) != VK_SUCCESS)
	{
		Throw(std::runtime_error("failed to create sampler"));
	}
}

Sampler::~Sampler()
{
	if (sampler_ != nullptr)
	{
		vkDestroySampler(device_.Handle(), sampler_, nullptr);
		sampler_ = nullptr;
	}
}

}
