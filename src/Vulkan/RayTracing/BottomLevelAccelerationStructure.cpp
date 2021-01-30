#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Vertex.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"

namespace Vulkan::RayTracing {

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(
	const class DeviceProcedures& deviceProcedures,
	const class RayTracingProperties& rayTracingProperties,
	const BottomLevelGeometry& geometries) :
	AccelerationStructure(deviceProcedures, rayTracingProperties),
	geometries_(geometries)
{
	buildGeometryInfo_.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometryInfo_.flags = flags_;
	buildGeometryInfo_.geometryCount = static_cast<uint32_t>(geometries_.Geometry().size());
	buildGeometryInfo_.pGeometries = geometries_.Geometry().data();
	buildGeometryInfo_.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildGeometryInfo_.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	buildGeometryInfo_.srcAccelerationStructure = nullptr;

	std::vector<uint32_t> maxPrimCount(geometries_.BuildOffsetInfo().size());

	for (size_t i = 0; i != maxPrimCount.size(); ++i)
	{
		maxPrimCount[i] = geometries_.BuildOffsetInfo()[i].primitiveCount;
	}
	
	buildSizesInfo_ = GetBuildSizes(maxPrimCount.data());
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept :
	AccelerationStructure(std::move(other)),
	geometries_(std::move(other.geometries_))
{
}

BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure()
{
}

void BottomLevelAccelerationStructure::Generate(
	VkCommandBuffer commandBuffer,
	Buffer& scratchBuffer,
	const VkDeviceSize scratchOffset,
	Buffer& resultBuffer,
	const VkDeviceSize resultOffset)
{
	// Create the acceleration structure.
	CreateAccelerationStructure(resultBuffer, resultOffset);

	// Build the actual bottom-level acceleration structure
	const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = geometries_.BuildOffsetInfo().data();

	buildGeometryInfo_.dstAccelerationStructure = Handle();
	buildGeometryInfo_.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

	deviceProcedures_.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildGeometryInfo_, &pBuildOffsetInfo);
}

}
