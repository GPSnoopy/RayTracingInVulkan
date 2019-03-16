#include "Fence.hpp"
#include "Device.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Fence::Fence(const class Device& device, const bool signaled) :
	device_(device)
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if (vkCreateFence(device.Handle(), &fenceInfo, nullptr, &fence_) != VK_SUCCESS)
	{

		Throw(std::runtime_error("failed to create fence"));
	}
}

Fence::Fence(Fence&& other) noexcept :
	device_(other.device_),
	fence_(other.fence_)
{
	other.fence_ = nullptr;
}

Fence::~Fence()
{
	if (fence_ != nullptr)
	{
		vkDestroyFence(device_.Handle(), fence_, nullptr);
		fence_ = nullptr;
	}
}

void Fence::Reset()
{
	if (vkResetFences(device_.Handle(), 1, &fence_) != VK_SUCCESS)
	{
		Throw(std::runtime_error("failed to reset fence"));
	}
}

void Fence::Wait(const uint64_t timeout) const
{
	if (vkWaitForFences(device_.Handle(), 1, &fence_, VK_TRUE, timeout) != VK_SUCCESS)
	{
		Throw(std::runtime_error("failed to wait for fence"));
	}
}

}
