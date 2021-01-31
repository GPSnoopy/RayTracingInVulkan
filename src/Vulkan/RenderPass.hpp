#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class DepthBuffer;
	class SwapChain;

	class RenderPass final
	{
	public:

		VULKAN_NON_COPIABLE(RenderPass)

		RenderPass(const SwapChain& swapChain, const DepthBuffer& depthBuffer, VkAttachmentLoadOp colorBufferLoadOp, VkAttachmentLoadOp depthBufferLoadOp);
		~RenderPass();

		const class SwapChain& SwapChain() const { return swapChain_; }
		const class DepthBuffer& DepthBuffer() const { return depthBuffer_; }

	private:

		const class SwapChain& swapChain_;
		const class DepthBuffer& depthBuffer_;

		VULKAN_HANDLE(VkRenderPass, renderPass_)
	};

}
