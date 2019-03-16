#pragma once

#include "Vulkan.hpp"
#include <vector>

namespace Vulkan
{
	class CommandPool;

	class CommandBuffers final
	{
	public:

		VULKAN_NON_COPIABLE(CommandBuffers)

		CommandBuffers(CommandPool& commandPool, uint32_t size);
		~CommandBuffers();

		uint32_t Size() const { return static_cast<uint32_t>(commandBuffers_.size()); }
		VkCommandBuffer& operator [] (const size_t i) { return commandBuffers_[i]; }

		VkCommandBuffer Begin(size_t i);
		void End(size_t);

	private:

		const CommandPool& commandPool_;

		std::vector<VkCommandBuffer> commandBuffers_;
	};

}
