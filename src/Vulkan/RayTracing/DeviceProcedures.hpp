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
				const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
				const VkAllocationCallbacks* pAllocator,
				VkAccelerationStructureKHR* pAccelerationStructure)>
			vkCreateAccelerationStructureKHR;

			const std::function<void(
				VkDevice device,
				VkAccelerationStructureKHR accelerationStructure,
				const VkAllocationCallbacks* pAllocator)>
			vkDestroyAccelerationStructureKHR;

			const std::function<void(
				VkDevice device,
				const VkAccelerationStructureMemoryRequirementsInfoKHR* pInfo,
				VkMemoryRequirements2KHR* pMemoryRequirements)>
			vkGetAccelerationStructureMemoryRequirementsKHR;

			const std::function<VkResult(
				VkDevice device,
				uint32_t bindInfoCount,
				const VkBindAccelerationStructureMemoryInfoKHR* pBindInfos)>
			vkBindAccelerationStructureMemoryKHR;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				uint32_t infoCount, 
				const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, 
				const VkAccelerationStructureBuildOffsetInfoKHR* const* ppOffsetInfos)>
			vkCmdBuildAccelerationStructureKHR;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				const VkCopyAccelerationStructureInfoKHR* pInfo)>
			vkCmdCopyAccelerationStructureKHR;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				const VkStridedBufferRegionKHR* pRaygenShaderBindingTable, 
				const VkStridedBufferRegionKHR* pMissShaderBindingTable, 
				const VkStridedBufferRegionKHR* pHitShaderBindingTable, 
				const VkStridedBufferRegionKHR* pCallableShaderBindingTable, 
				uint32_t width, 
				uint32_t height, 
				uint32_t depth)>
			vkCmdTraceRaysKHR;

			const std::function<VkResult(
				VkDevice device,
				VkPipelineCache pipelineCache,
				uint32_t createInfoCount,
				const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
				const VkAllocationCallbacks* pAllocator,
				VkPipeline* pPipelines)>
			vkCreateRayTracingPipelinesKHR;

			const std::function<VkResult(
				VkDevice device,
				VkPipeline pipeline,
				uint32_t firstGroup,
				uint32_t groupCount,
				size_t dataSize,
				void* pData)>
			vkGetRayTracingShaderGroupHandlesKHR;

			const std::function<VkDeviceAddress(
				VkDevice device, 
				const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)>
			vkGetAccelerationStructureDeviceAddressKHR;

			const std::function<void(
				VkCommandBuffer commandBuffer,
				uint32_t accelerationStructureCount,
				const VkAccelerationStructureKHR* pAccelerationStructures,
				VkQueryType queryType,
				VkQueryPool queryPool,
				uint32_t firstQuery)>
			vkCmdWriteAccelerationStructuresPropertiesKHR;
			
		private:

			const class Device& device_;
		};
	}
}
