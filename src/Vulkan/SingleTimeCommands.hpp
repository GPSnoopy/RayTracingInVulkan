#pragma once

#include "Vulkan.hpp"
#include "CommandBuffers.hpp"
#include "CommandPool.hpp"
#include "Device.hpp"
#include <functional>

namespace Vulkan
{
	class SingleTimeCommands final
	{
	public:

		static void Submit(CommandPool& commandPool, const std::function<void(VkCommandBuffer)>& action)
		{
			CommandBuffers commandBuffers(commandPool, 1);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

			action(commandBuffers[0]);

			vkEndCommandBuffer(commandBuffers[0]);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffers[0];

			const auto graphicsQueue = commandPool.Device().GraphicsQueue();

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(graphicsQueue);
		}
	};

}
