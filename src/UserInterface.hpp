#pragma once
#include "Vulkan/Vulkan.hpp"
#include <memory>

namespace Vulkan
{
	class CommandPool;
	class DepthBuffer;
	class DescriptorPool;
	class FrameBuffer;
	class RenderPass;
	class SwapChain;
}

struct UserSettings;

struct Statistics final
{
	VkExtent2D FramebufferSize;
	float FrameRate;
	float RayRate;
	uint32_t TotalSamples;
};

class UserInterface final
{
public:

	VULKAN_NON_COPIABLE(UserInterface)

	UserInterface(
		Vulkan::CommandPool& commandPool, 
		const Vulkan::SwapChain& swapChain, 
		const Vulkan::DepthBuffer& depthBuffer,
		UserSettings& userSettings);
	~UserInterface();

	void Render(VkCommandBuffer commandBuffer, const Vulkan::FrameBuffer& frameBuffer, const Statistics& statistics);

	bool WantsToCaptureKeyboard() const;
	bool WantsToCaptureMouse() const;

	UserSettings& Settings() { return userSettings_; }

private:

	void DrawSettings();
	void DrawOverlay(const Statistics& statistics);

	std::unique_ptr<Vulkan::DescriptorPool> descriptorPool_;
	std::unique_ptr<Vulkan::RenderPass> renderPass_;
	UserSettings& userSettings_;
};
