#pragma once

#include "WindowConfig.hpp"
#include "Vulkan.hpp"
#include <functional>
#include <vector>

namespace Vulkan
{

	class Window final
	{
	public:

		VULKAN_NON_COPIABLE(Window)

		explicit Window(const WindowConfig& config);
		~Window();

		const WindowConfig& Config() const { return config_; }
		GLFWwindow* Handle() const { return window_; }

		std::vector<const char*> GetRequiredInstanceExtensions() const;
		float ContentScale() const;
		double Time() const;
		VkExtent2D FramebufferSize() const;
		VkExtent2D WindowSize() const;

		std::function<void()> DrawFrame;

		std::function<void(int key, int scancode, int action, int mods)> OnKey;
		std::function<void(double xpos, double ypos)> OnCursorPosition;
		std::function<void(int button, int action, int mods)> OnMouseButton;

		void Close() const;
		bool IsMinimized() const;
		void Run() const;
		void WaitForEvents() const;

	private:

		const WindowConfig config_;
		GLFWwindow* window_{};
	};

}
