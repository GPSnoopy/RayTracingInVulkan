#include "Surface.hpp"
#include "Instance.hpp"
#include "Window.hpp"
#include "Utilities/Exception.hpp"

namespace Vulkan {

Surface::Surface(const class Instance& instance) :
	instance_(instance)
{
	if (glfwCreateWindowSurface(instance.Handle(), instance.Window().Handle(), nullptr, &surface_) != VK_SUCCESS)
	{
		Throw(std::runtime_error("failed to create window surface"));
	}
}

Surface::~Surface()
{
	if (surface_ != nullptr)
	{
		vkDestroySurfaceKHR(instance_.Handle(), surface_, nullptr);
		surface_ = nullptr;
	}
}

}
