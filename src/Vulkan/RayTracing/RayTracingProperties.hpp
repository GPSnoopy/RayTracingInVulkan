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

			uint32_t MaxDescriptorSetAccelerationStructures() const { return props_.maxDescriptorSetAccelerationStructures; }
			uint64_t MaxGeometryCount() const { return props_.maxGeometryCount; }
			uint64_t MaxInstanceCount() const { return props_.maxInstanceCount; }
			uint32_t MaxRecursionDepth() const { return props_.maxRecursionDepth; }
			uint32_t MaxShaderGroupStride() const { return props_.maxShaderGroupStride; }
			uint64_t MaxTriangleCount() const { return props_.maxTriangleCount; }
			uint32_t ShaderGroupBaseAlignment() const { return props_.shaderGroupBaseAlignment; }
			uint32_t ShaderGroupHandleSize() const { return props_.shaderGroupHandleSize; }

		private:

			const class Device& device_;
			VkPhysicalDeviceRayTracingPropertiesNV props_{};
		};
	}
}
