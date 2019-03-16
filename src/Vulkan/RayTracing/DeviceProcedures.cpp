#include "DeviceProcedures.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Device.hpp"
#include <string>

namespace Vulkan::RayTracing
{

namespace
{
	template <class Func>
	Func GetProcedure(const Device& device, const char* const name)
	{
		const auto func = reinterpret_cast<Func>(vkGetDeviceProcAddr(device.Handle(), name));
		if (func == nullptr)
		{
			Throw(std::runtime_error(std::string("failed to get address of '") + name + "'"));
		}

		return func;
	}
}


DeviceProcedures::DeviceProcedures(const class Device& device) :
	vkCreateAccelerationStructureNV(GetProcedure<PFN_vkCreateAccelerationStructureNV>(device, "vkCreateAccelerationStructureNV")),
	vkDestroyAccelerationStructureNV(GetProcedure<PFN_vkDestroyAccelerationStructureNV>(device, "vkDestroyAccelerationStructureNV")),
	vkGetAccelerationStructureMemoryRequirementsNV(GetProcedure<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(device, "vkGetAccelerationStructureMemoryRequirementsNV")),
	vkBindAccelerationStructureMemoryNV(GetProcedure<PFN_vkBindAccelerationStructureMemoryNV>(device, "vkBindAccelerationStructureMemoryNV")),
	vkCmdBuildAccelerationStructureNV(GetProcedure<PFN_vkCmdBuildAccelerationStructureNV>(device, "vkCmdBuildAccelerationStructureNV")),
	vkCmdCopyAccelerationStructureNV(GetProcedure<PFN_vkCmdCopyAccelerationStructureNV>(device, "vkCmdCopyAccelerationStructureNV")),
	vkCmdTraceRaysNV(GetProcedure<PFN_vkCmdTraceRaysNV>(device, "vkCmdTraceRaysNV")),
	vkCreateRayTracingPipelinesNV(GetProcedure<PFN_vkCreateRayTracingPipelinesNV>(device, "vkCreateRayTracingPipelinesNV")),
	vkGetRayTracingShaderGroupHandlesNV(GetProcedure<PFN_vkGetRayTracingShaderGroupHandlesNV>(device, "vkGetRayTracingShaderGroupHandlesNV")),
	vkGetAccelerationStructureHandleNV(GetProcedure<PFN_vkGetAccelerationStructureHandleNV>(device, "vkGetAccelerationStructureHandleNV")),
	vkCmdWriteAccelerationStructuresPropertiesNV(GetProcedure<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(device, "vkCmdWriteAccelerationStructuresPropertiesNV")),
	vkCompileDeferredNV(GetProcedure<PFN_vkCompileDeferredNV>(device, "vkCompileDeferredNV")),
	device_(device)
{
}

DeviceProcedures::~DeviceProcedures()
{
}

}
