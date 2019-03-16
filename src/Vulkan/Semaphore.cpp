#include "Semaphore.hpp"
#include "Device.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Semaphore::Semaphore(const class Device& device) :
	device_(device)
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device.Handle(), &semaphoreInfo, nullptr, &semaphore_) != VK_SUCCESS)
	{

		Throw(std::runtime_error("failed to create semaphores"));
	}
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
