#include "FrameBuffer.hpp"
#include "DepthBuffer.hpp"
#include "Device.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"
#include <array>

namespace Vulkan {

FrameBuffer::FrameBuffer(const class ImageView& imageView, const class RenderPass& renderPass) :
	imageView_(imageView),
	renderPass_(renderPass)
{
	std::array<VkImageView, 2> attachments =
	{
		imageView.Handle(),
		renderPass.DepthBuffer().ImageView().Handle()
	};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass.Handle();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = renderPass.SwapChain().Extent().width;
	framebufferInfo.height = renderPass.SwapChain().Extent().height;
	framebufferInfo.layers = 1;

	Check(vkCreateFramebuffer(imageView_.Device().Handle(), &framebufferInfo, nullptr, &framebuffer_),
		"create framebuffer");
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept :
	imageView_(other.imageView_),
	renderPass_(other.renderPass_),
	framebuffer_(other.framebuffer_)
{
	other.framebuffer_ = nullptr;
}

FrameBuffer::~FrameBuffer()
{
	if (framebuffer_ != nullptr)
	{
		vkDestroyFramebuffer(imageView_.Device().Handle(), framebuffer_, nullptr);
		framebuffer_ = nullptr;
	}
}

}
