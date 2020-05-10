#pragma once

#include "ModelViewController.hpp"
#include "SceneList.hpp"
#include "UserSettings.hpp"
#include "Vulkan/RayTracing/Application.hpp"

class RayTracer final : public Vulkan::RayTracing::Application
{
public:

	VULKAN_NON_COPIABLE(RayTracer)

	RayTracer(const UserSettings& userSettings, const Vulkan::WindowConfig& windowConfig, VkPresentModeKHR presentMode);
	~RayTracer();

protected:

	const Assets::Scene& GetScene() const override { return *scene_; }
	Assets::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const override;

	void SetPhysicalDevice(
		VkPhysicalDevice physicalDevice, 
		std::vector<const char*>& requiredExtensions, 
		VkPhysicalDeviceFeatures& deviceFeatures, 
		void* nextDeviceFeatures) override;

	void OnDeviceSet() override;
	void CreateSwapChain() override;
	void DeleteSwapChain() override;
	void DrawFrame() override;
	void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

	void OnKey(int key, int scancode, int action, int mods) override;
	void OnCursorPosition(double xpos, double ypos) override;
	void OnMouseButton(int button, int action, int mods) override;
	void OnScroll(double xoffset, double yoffset) override;

private:

	void LoadScene(uint32_t sceneIndex);
	void CheckAndUpdateBenchmarkState(double prevTime);
	void CheckFramebufferSize() const;

	uint32_t sceneIndex_{};
	UserSettings userSettings_{};
	UserSettings previousSettings_{};
	SceneList::CameraInitialSate cameraInitialSate_{};
	ModelViewController modelViewController_{};

	std::unique_ptr<const Assets::Scene> scene_;
	std::unique_ptr<class UserInterface> userInterface_;

	double time_{};

	uint32_t totalNumberOfSamples_{};
	uint32_t numberOfSamples_{};
	bool resetAccumulation_{};

	// Benchmark stats
	double sceneInitialTime_{};
	double periodInitialTime_{};
	uint32_t periodTotalFrames_{};
};
