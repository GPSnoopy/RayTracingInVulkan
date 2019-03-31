#include "Semaphore.hpp"
#include "Device.hpp"

namespace Vulkan {

Semaphore::Semaphore(const class Device& device) :
	device_(device)
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	Check(vkCreateSemaphore(device.Handle(), &semaphoreInfo, nullptr, &semaphore_),
		"create semaphores");
}

Semaphore::Semaphore(Semaphore&& other) noexcept :
	device_(other.device_),
	semaphore_(other.semaphore_)
{
	other.semaphore_ = nullptr;
}

Semaphore::~Semaphore()
{
	if (semaphore_ != nullptr)
	{
		vkDestroySemaphore(device_.Handle(), semaphore_, nullptr);
		semaphore_ = nullptr;
	}
}

}
