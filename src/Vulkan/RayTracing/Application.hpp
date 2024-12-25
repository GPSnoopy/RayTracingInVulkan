#pragma once

#include "Vulkan/Application.hpp"
#include "RayTracingProperties.hpp"

namespace Vulkan
{
	class CommandBuffers;
	class Buffer;
	class DeviceMemory;
	class Image;
	class ImageView;
}

namespace Vulkan::RayTracing
{
	class Application : public Vulkan::Application
	{
	public:

		VULKAN_NON_COPIABLE(Application);

	protected:

		Application(const WindowConfig& windowConfig, VkPresentModeKHR presentMode, bool enableValidationLayers);
		~Application();

		void SetPhysicalDevice(VkPhysicalDevice physicalDevice,
			std::vector<const char*>& requiredExtensions,
			VkPhysicalDeviceFeatures& deviceFeatures,
			void* nextDeviceFeatures) override;
		
		void OnDeviceSet() override;
		void CreateAccelerationStructures();
		void DeleteAccelerationStructures();
		void CreateSwapChain() override;
		void DeleteSwapChain() override;
		void Render(VkCommandBuffer commandBuffer, size_t currentFrame, uint32_t imageIndex) override;
			   
	private:

		void CreateBottomLevelStructures(VkCommandBuffer commandBuffer);
		void CreateTopLevelStructures(VkCommandBuffer commandBuffer);
		void CreateOutputImage();

		std::unique_ptr<class DeviceProcedures> deviceProcedures_;
		std::unique_ptr<class RayTracingProperties> rayTracingProperties_;

		std::vector<class BottomLevelAccelerationStructure> bottomAs_;
		std::unique_ptr<Buffer> bottomBuffer_;
		std::unique_ptr<DeviceMemory> bottomBufferMemory_;
		std::unique_ptr<Buffer> bottomScratchBuffer_;
		std::unique_ptr<DeviceMemory> bottomScratchBufferMemory_;
		std::vector<class TopLevelAccelerationStructure> topAs_;
		std::unique_ptr<Buffer> topBuffer_;
		std::unique_ptr<DeviceMemory> topBufferMemory_;
		std::unique_ptr<Buffer> topScratchBuffer_;
		std::unique_ptr<DeviceMemory> topScratchBufferMemory_;
		std::unique_ptr<Buffer> instancesBuffer_;
		std::unique_ptr<DeviceMemory> instancesBufferMemory_;

		std::unique_ptr<Image> accumulationImage_;
		std::unique_ptr<DeviceMemory> accumulationImageMemory_;
		std::unique_ptr<ImageView> accumulationImageView_;

		std::unique_ptr<Image> outputImage_;
		std::unique_ptr<DeviceMemory> outputImageMemory_;
		std::unique_ptr<ImageView> outputImageView_;
		
		std::unique_ptr<class RayTracingPipeline> rayTracingPipeline_;
		std::unique_ptr<class ShaderBindingTable> shaderBindingTable_;
	};

}
