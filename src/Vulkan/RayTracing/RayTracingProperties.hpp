#pragma once

#include "Vulkan/Vulkan.hpp"

namespace Vulkan
{
	class Device;

	namespace RayTracing
	{
		class RayTracingProperties final
		{
		public:

			explicit RayTracingProperties(const Device& device);

			const class Device& Device() const { return device_; }

			uint32_t MaxDescriptorSetAccelerationStructures() const { return accelProps_.maxDescriptorSetAccelerationStructures; }
			uint64_t MaxGeometryCount() const { return accelProps_.maxGeometryCount; }
			uint64_t MaxInstanceCount() const { return accelProps_.maxInstanceCount; }
			uint64_t MaxPrimitiveCount() const { return accelProps_.maxPrimitiveCount; }
			uint32_t MaxRayRecursionDepth() const { return pipelineProps_.maxRayRecursionDepth; }
			uint32_t MaxShaderGroupStride() const { return pipelineProps_.maxShaderGroupStride; }
			uint32_t MinAccelerationStructureScratchOffsetAlignment() const { return accelProps_.minAccelerationStructureScratchOffsetAlignment; }
			uint32_t ShaderGroupBaseAlignment() const { return pipelineProps_.shaderGroupBaseAlignment; }
			uint32_t ShaderGroupHandleCaptureReplaySize() const { return pipelineProps_.shaderGroupHandleCaptureReplaySize; }
			uint32_t ShaderGroupHandleSize() const { return pipelineProps_.shaderGroupHandleSize; }

		private:

			const class Device& device_;
			VkPhysicalDeviceAccelerationStructurePropertiesKHR accelProps_{};
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR pipelineProps_{};
		};
	}
}
