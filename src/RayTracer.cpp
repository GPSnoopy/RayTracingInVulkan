#include "RayTracer.hpp"
#include "UserInterface.hpp"
#include "UserSettings.hpp"
#include "Assets/Model.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Texture.hpp"
#include "Assets/UniformBuffer.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/Glm.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Window.hpp"
#include <iostream>
#include <sstream>

namespace
{
	const bool EnableValidationLayers =
#ifdef NDEBUG
		false;
#else
		true;
#endif
}

RayTracer::RayTracer(const UserSettings& userSettings, const Vulkan::WindowConfig& windowConfig, const bool vsync) :
	Application(windowConfig, vsync, EnableValidationLayers),
	userSettings_(userSettings)
{
	// Check the framebuffer size when requesting a fullscreen window, as it's not guaranteed to match.
	const auto fbSize = Window().FramebufferSize();
	
	if (windowConfig.Fullscreen && (fbSize.width != windowConfig.Width || fbSize.height != windowConfig.Height))
	{
		std::ostringstream out;
		out << "framebuffer fullscreen size mismatch (requested: ";
		out << windowConfig.Width << "x" << windowConfig.Height;
		out << ", got: ";
		out << fbSize.width << "x" << fbSize.height << ")";
		
		Throw(std::runtime_error(out.str()));
	}
}

RayTracer::~RayTracer()
{
	scene_.reset();
}

Assets::UniformBufferObject RayTracer::GetUniformBufferObject(const VkExtent2D extent) const
{
	const auto cameraRotX = static_cast<float>(cameraY_ / 300.0);
	const auto cameraRotY = static_cast<float>(cameraX_ / 300.0);

	const auto& init = cameraInitialSate_;
	const auto view = init.ModelView;
	const auto model =
		glm::rotate(glm::mat4(1.0f), cameraRotY * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), cameraRotX * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	Assets::UniformBufferObject ubo = {};
	ubo.ModelView = view*model;
	ubo.Projection = glm::perspective(glm::radians(userSettings_.FieldOfView), extent.width / static_cast<float>(extent.height), 0.1f, 10000.0f);
	ubo.Projection[1][1] *= -1; // Inverting Y for Vulkan, https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
	ubo.ModelViewInverse = glm::inverse(ubo.ModelView);
	ubo.ProjectionInverse = glm::inverse(ubo.Projection);
	ubo.Aperture = userSettings_.Aperture;
	ubo.FocusDistance = userSettings_.FocusDistance;
	ubo.TotalNumberOfSamples = totalNumberOfSamples_;
	ubo.NumberOfSamples = numberOfSamples_;
	ubo.NumberOfBounces = userSettings_.NumberOfBounces;
	ubo.RandomSeed = 1;
	ubo.GammaCorrection = userSettings_.GammaCorrection;
	ubo.HasSky = init.HasSky;

	return ubo;
}

void RayTracer::OnDeviceSet()
{
	Application::OnDeviceSet();

	LoadScene(userSettings_.SceneIndex);
	CreateAccelerationStructures();
}

void RayTracer::CreateSwapChain()
{
	Application::CreateSwapChain();

	userInterface_.reset(new UserInterface(CommandPool(), SwapChain(), DepthBuffer(), userSettings_));
	resetAccumulation_ = true;
}

void RayTracer::DeleteSwapChain()
{
	userInterface_.reset();

	Application::DeleteSwapChain();
}

void RayTracer::DrawFrame()
{
	// Check if the scene has been changed by the user.
	if (sceneIndex_ != static_cast<uint32_t>(userSettings_.SceneIndex))
	{
		Device().WaitIdle();
		DeleteSwapChain();
		DeleteAccelerationStructures();
		LoadScene(userSettings_.SceneIndex);
		CreateAccelerationStructures();
		CreateSwapChain();
		return;
	}

	// Check if the accumulation buffer needs to be reset.
	if (resetAccumulation_ || 
		userSettings_.RequiresAccumulationReset(previousSettings_) || 
		!userSettings_.AccumulateRays)
	{
		totalNumberOfSamples_ = 0;
		resetAccumulation_ = false;
	}

	previousSettings_ = userSettings_;

	// Keep track of our sample count.
	numberOfSamples_ = glm::clamp(userSettings_.MaxNumberOfSamples - totalNumberOfSamples_, 0u, userSettings_.NumberOfSamples);
	totalNumberOfSamples_ += numberOfSamples_;

	Application::DrawFrame();
}

void RayTracer::Render(VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
	// Record delta time between calls to Render.
	const auto prevTime = time_;
	time_ = Window().Time();
	const auto deltaTime = time_ - prevTime;

	// Check the current state of the benchmark, update it for the new frame.
	CheckAndUpdateBenchmarkState(prevTime);

	// Render the scene
	userSettings_.IsRayTraced
		? Vulkan::RayTracing::Application::Render(commandBuffer, imageIndex)
		: Vulkan::Application::Render(commandBuffer, imageIndex);

	// Render the UI
	Statistics stats = {};
	stats.FramebufferSize = Window().FramebufferSize();
	stats.FrameRate = static_cast<float>(1 / deltaTime);

	if (userSettings_.IsRayTraced)
	{
		const auto extent = SwapChain().Extent();

		stats.RayRate = static_cast<float>(
			double(extent.width*extent.height)*numberOfSamples_
			/ (deltaTime * 1000000000));

		stats.TotalSamples = totalNumberOfSamples_;
	}

	userInterface_->Render(commandBuffer, SwapChainFrameBuffer(imageIndex), stats);
}

void RayTracer::OnKey(int key, int scancode, int action, int mods)
{
	if (userInterface_->WantsToCaptureKeyboard())
	{
		return;
	}

	if (action == GLFW_PRESS)
	{		
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			Window().Close();
			break;
		default:
			break;
		}

		if (!userSettings_.Benchmark)
		{
			switch (key)
			{
			case GLFW_KEY_F1:
				userSettings_.ShowSettings = !userSettings_.ShowSettings;
				break;
			case GLFW_KEY_F2:
				userSettings_.ShowOverlay = !userSettings_.ShowOverlay;
				break;
			case GLFW_KEY_R:
				userSettings_.IsRayTraced = !userSettings_.IsRayTraced;
				break;
			case GLFW_KEY_W:
				isWireFrame_ = !isWireFrame_;
				break;
			default:
				break;
			}
		}
	}
}

void RayTracer::OnCursorPosition(const double xpos, const double ypos)
{
	if (userSettings_.Benchmark ||
		userInterface_->WantsToCaptureKeyboard() || 
		userInterface_->WantsToCaptureMouse())
	{
		return;
	}

	if (mouseLeftPressed_)
	{
		const auto deltaX = static_cast<float>(xpos - mouseX_);
		const auto deltaY = static_cast<float>(ypos - mouseY_);

		cameraX_ += deltaX;
		cameraY_ += deltaY;

		resetAccumulation_ = true;
	}

	mouseX_ = xpos;
	mouseY_ = ypos;
}

void RayTracer::OnMouseButton(const int button, const int action, const int mods)
{
	if (userSettings_.Benchmark || 
		userInterface_->WantsToCaptureMouse())
	{
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		mouseLeftPressed_ = action == GLFW_PRESS;
	}
}

void RayTracer::LoadScene(const uint32_t sceneIndex)
{
	auto [models, textures] = SceneList::AllScenes[sceneIndex].second(cameraInitialSate_);

	// If there are no texture, add a dummy one. It makes the pipeline setup a lot easier.
	if (textures.empty())
	{
		textures.push_back(Assets::Texture::LoadTexture("../assets/textures/white.png", Vulkan::SamplerConfig()));
	}
	
	scene_.reset(new Assets::Scene(CommandPool(), std::move(models), std::move(textures), true));
	sceneIndex_ = sceneIndex;

	userSettings_.FieldOfView = cameraInitialSate_.FieldOfView;
	userSettings_.Aperture = cameraInitialSate_.Aperture;
	userSettings_.FocusDistance = cameraInitialSate_.FocusDistance;
	userSettings_.GammaCorrection = cameraInitialSate_.GammaCorrection;

	cameraX_ = 0;
	cameraY_ = 0;

	periodTotalFrames_ = 0;
	resetAccumulation_ = true;
}

void RayTracer::CheckAndUpdateBenchmarkState(double prevTime)
{
	if (!userSettings_.Benchmark)
	{
		return;
	}
	
	// Initialise scene benchmark timers
	if (periodTotalFrames_ == 0)
	{
		std::cout << std::endl;
		std::cout << "Benchmark: Start scene #" << sceneIndex_ << " '" << SceneList::AllScenes[sceneIndex_].first << "'" << std::endl;
		sceneInitialTime_ = time_;
		periodInitialTime_ = time_;
	}

	// Print out the frame rate at regular intervals.
	{
		const double period = 5;
		const double prevTotalTime = prevTime - periodInitialTime_;
		const double totalTime = time_ - periodInitialTime_;

		if (periodTotalFrames_ != 0 && static_cast<uint64_t>(prevTotalTime / period) != static_cast<uint64_t>(totalTime / period))
		{
			std::cout << "Benchmark: " << periodTotalFrames_ / totalTime << " fps" << std::endl;
			periodInitialTime_ = time_;
			periodTotalFrames_ = 0;
		}

		periodTotalFrames_++;
	}

	// If in benchmark mode, bail out from the scene if we've reached the time or sample limit.
	{
		const bool timeLimitReached = periodTotalFrames_ != 0 && Window().Time() - sceneInitialTime_ > userSettings_.BenchmarkMaxTime;
		const bool sampleLimitReached = numberOfSamples_ == 0;

		if (timeLimitReached || sampleLimitReached)
		{
			if (!userSettings_.BenchmarkNextScenes || static_cast<size_t>(userSettings_.SceneIndex) == SceneList::AllScenes.size() - 1)
			{
				Window().Close();
			}

			std::cout << std::endl;
			userSettings_.SceneIndex += 1;
		}
	}
}
