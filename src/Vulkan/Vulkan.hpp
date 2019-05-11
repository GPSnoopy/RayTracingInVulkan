#pragma once

#define NOMINMAX
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef APIENTRY

#define VULKAN_NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	VulkanHandleType Handle() const { return name; } \
private: \
	VulkanHandleType name{};

namespace Vulkan
{
	void Check(VkResult result, const char* operation);
	const char* ToString(VkResult result);
}
