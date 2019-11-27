#include "Window.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/StbImage.hpp"
#include <iostream>

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

namespace Vulkan {

namespace
{
	void GlfwErrorCallback(const int error, const char* const description)
	{
		std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
	}

	void GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
	{
		const auto this_ = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (this_->OnKey)
		{
			this_->OnKey(key, scancode, action, mods);
		}
	}

	void GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		const auto this_ = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (this_->OnCursorPosition)
		{
			this_->OnCursorPosition(xpos, ypos);
		}
	}

	void GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		const auto this_ = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (this_->OnMouseButton)
		{
			this_->OnMouseButton(button, action, mods);
		}
	}
}

Window::Window(const WindowConfig& config) :
	config_(config)
{
	glfwSetErrorCallback(GlfwErrorCallback);

	if (!glfwInit())
	{
		Throw(std::runtime_error("glfwInit() failed"));
	}

	if (!glfwVulkanSupported())
	{
		Throw(std::runtime_error("glfwVulkanSupported() failed"));
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, config.Resizable ? GLFW_TRUE : GLFW_FALSE);

	const auto monitor = config.Fullscreen ? glfwGetPrimaryMonitor() : nullptr;

	window_ = glfwCreateWindow(config.Width, config.Height, config.Title.c_str(), monitor, nullptr);
	if (window_ == nullptr)
	{
		Throw(std::runtime_error("failed to create window"));
	}

	GLFWimage icon;
	icon.pixels = stbi_load("../assets/textures/Vulkan.png", &icon.width, &icon.height, nullptr, 4);
	if (icon.pixels == nullptr)
	{
		Throw(std::runtime_error("failed to load icon"));
	}

	glfwSetWindowIcon(window_, 1, &icon);
	stbi_image_free(icon.pixels);

	if (config.CursorDisabled)
	{
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, GlfwKeyCallback);
	glfwSetCursorPosCallback(window_, GlfwCursorPositionCallback);
	glfwSetMouseButtonCallback(window_, GlfwMouseButtonCallback);
}

Window::~Window()
{
	if (window_ != nullptr)
	{
		glfwDestroyWindow(window_);
		window_ = nullptr;
	}

	glfwTerminate();
	glfwSetErrorCallback(nullptr);
}

std::vector<const char*> Window::GetRequiredInstanceExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

float Window::ContentScale() const
{
	float xscale;
	float yscale;
	glfwGetWindowContentScale(window_, &xscale, &yscale);

	return xscale;
}

double Window::Time() const
{
	return glfwGetTime();
}

VkExtent2D Window::FramebufferSize() const
{
	int width, height;
	glfwGetFramebufferSize(window_, &width, &height);
	return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

VkExtent2D Window::WindowSize() const
{
	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void Window::Close() const
{
	glfwSetWindowShouldClose(window_, 1);
}

bool Window::IsMinimized() const
{
	const auto size = FramebufferSize();
	return size.height == 0 && size.width == 0;
}

void Window::Run() const
{
	glfwSetTime(0.0);
	DEBUG_MSG("Run window \n--------------------------");
	while (!glfwWindowShouldClose(window_))
	{
		glfwPollEvents(); //render
		if (DrawFrame)
		{
			DrawFrame();
		}
		DEBUG_MSG("---------------");
	}
}

void Window::WaitForEvents() const
{
	glfwWaitEvents();
}

}
