#pragma once

#include "Vulkan/Vulkan.hpp"
#include <functional>

namespace Vulkan
{
	class Device;

	namespace RayTracing
	{
		class DeviceProcedures final
		{
		public:

			VULKAN_NON_COPIABLE(DeviceProcedures)

			explicit DeviceProcedures(const Device& device);
			~DeviceProcedures();

			const class Device& Device() const { return device_; }

			const std::function<VkResult(
				VkDevice device,
				const VkAccelerationStructureCreateInfoNV* pCreateInfo,
				const VkAllocationCallbacks* pAllocator,
				VkAccelerationStructureNV* pAccelerationStructure)>
			vkCreateAccelerationStructureNV;

			const std::function<void(
				VkDevice device,
				VkAccelerationStructureNV accelerationStructure,
				const VkAllocationCallbacks* pAllocator)>
			vkDestroyAccelerationStructureNV;

			const std::function<void(
				VkDevice device,
				const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
				VkMemoryRequirements2KHR* pMemoryRequirements)>
			vkGetAccelerationStructureMemoryRequirementsNV;

			const std::function<VkResult(
				VkDevice device,
				uint32_t bindInfoCount,
				const VkBindAccelerationStructureMemoryInfoNV* pBindInfos)>
			vkBindAccelerationStructureMemoryNV;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				const VkAccelerationStructureInfoNV* pInfo,
				VkBuffer instanceData,
				VkDeviceSize instanceOffset,
				VkBool32 update,
				VkAccelerationStructureNV dst,
				VkAccelerationStructureNV src,
				VkBuffer scratch,
				VkDeviceSize scratchOffset)>
			vkCmdBuildAccelerationStructureNV;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				VkAccelerationStructureNV dst,
				VkAccelerationStructureNV src,
				VkCopyAccelerationStructureModeNV mode)>
			vkCmdCopyAccelerationStructureNV;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				VkBuffer raygenShaderBindingTableBuffer,
				VkDeviceSize raygenShaderBindingOffset,
				VkBuffer missShaderBindingTableBuffer,
				VkDeviceSize missShaderBindingOffset,
				VkDeviceSize missShaderBindingStride,
				VkBuffer hitShaderBindingTableBuffer,
				VkDeviceSize hitShaderBindingOffset,
				VkDeviceSize hitShaderBindingStride,
				VkBuffer callableShaderBindingTableBuffer,
				VkDeviceSize callableShaderBindingOffset,
				VkDeviceSize callableShaderBindingStride,
				uint32_t width,
				uint32_t height,
				uint32_t depth)>
			vkCmdTraceRaysNV;

			const std::function<VkResult(
				VkDevice device,
				VkPipelineCache pipelineCache,
				uint32_t createInfoCount,
				const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
				const VkAllocationCallbacks* pAllocator,
				VkPipeline* pPipelines)>
			vkCreateRayTracingPipelinesNV;

			const std::function<VkResult(
				VkDevice device,
				VkPipeline pipeline,
				uint32_t firstGroup,
				uint32_t groupCount,
				size_t dataSize,
				void* pData)>
			vkGetRayTracingShaderGroupHandlesNV;

			const std::function<VkResult(
				VkDevice device,
				VkAccelerationStructureNV accelerationStructure,
				size_t dataSize,
				void* pData)>
			vkGetAccelerationStructureHandleNV;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				uint32_t accelerationStructureCount,
				const VkAccelerationStructureNV* pAccelerationStructures,
				VkQueryType queryType,
				VkQueryPool queryPool,
				uint32_t firstQuery)>
			vkCmdWriteAccelerationStructuresPropertiesNV;

			const std::function<VkResult(
				VkDevice device,
				VkPipeline pipeline,
				uint32_t shader)>
			vkCompileDeferredNV;

		private:

			const class Device& device_;
		};
	}
}
