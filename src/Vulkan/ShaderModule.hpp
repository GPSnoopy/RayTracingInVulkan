#pragma once

#include "Vulkan.hpp"
#include <string>
#include <vector>

namespace Vulkan
{
	class Device;

	class ShaderModule final
	{
	public:

		VULKAN_NON_COPIABLE(ShaderModule)

		ShaderModule(const Device& device, const std::string& filename);
		ShaderModule(const Device& device, const std::vector<char>& code);
		~ShaderModule();

		const class Device& Device() const { return device_; }

		VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

	private:

		static std::vector<char> ReadFile(const std::string& filename);

		const class Device& device_;

		VULKAN_HANDLE(VkShaderModule, shaderModule_)
	};

}
