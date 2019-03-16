#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class ImageView;
	class RenderPass;

	class FrameBuffer final
	{
	public:

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator = (const FrameBuffer&) = delete;
		FrameBuffer& operator = (FrameBuffer&&) = delete;

		explicit FrameBuffer(const ImageView& imageView, const RenderPass& renderPass);
		FrameBuffer(FrameBuffer&& other) noexcept;
		~FrameBuffer();

		const class ImageView& ImageView() const { return imageView_; }
		const class RenderPass& RenderPass() const { return renderPass_; }

	private:

		const class ImageView& imageView_;
		const class RenderPass& renderPass_;

		VULKAN_HANDLE(VkFramebuffer, framebuffer_)
	};

}
