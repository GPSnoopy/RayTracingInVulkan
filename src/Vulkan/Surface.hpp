#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
	class Instance;
	class Window;

	class Surface final
	{
	public:

		VULKAN_NON_COPIABLE(Surface)

		explicit Surface(const Instance& instance);
		~Surface();

		const class Instance& Instance() const { return instance_; }

	private:

		const class Instance& instance_;

		VULKAN_HANDLE(VkSurfaceKHR, surface_)
	};

}
