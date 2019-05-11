#include "Scene.hpp"
#include "Model.hpp"
#include "Sphere.hpp"
//#include "TextureImage.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Utilities/Exception.hpp"
#include <cstring>

namespace Assets {

namespace
{

	template <class T>
	void CopyFromStagingBuffer(Vulkan::CommandPool& commandPool, Vulkan::Buffer& dstBuffer, const std::vector<T>& content)
	{
		const auto& device = commandPool.Device();
		const auto contentSize = sizeof(content[0]) * content.size();

		// Create a temporary host-visible staging buffer.
		auto stagingBuffer = std::make_unique<Vulkan::Buffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Copy the host data into the staging buffer.
		const auto data = stagingBufferMemory.Map(0, contentSize);
		std::memcpy(data, content.data(), contentSize);
		stagingBufferMemory.Unmap();

		// Copy the staging buffer to the device buffer.
		dstBuffer.CopyFrom(commandPool, *stagingBuffer, contentSize);

		// Delete the buffer before the memory
		stagingBuffer.reset();
	}

	template <class T>
	void CreateDeviceBuffer(
		Vulkan::CommandPool& commandPool,
		const VkBufferUsageFlags usage, 
		const std::vector<T>& content,
		std::unique_ptr<Vulkan::Buffer>& buffer,
		std::unique_ptr<Vulkan::DeviceMemory>& memory)
	{
		const auto& device = commandPool.Device();
		const auto contentSize = sizeof(content[0]) * content.size();

		buffer.reset(new Vulkan::Buffer(device, contentSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage));
		memory.reset(new Vulkan::DeviceMemory(buffer->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

		CopyFromStagingBuffer(commandPool, *buffer, content);
	}

}

Scene::Scene(Vulkan::CommandPool& commandPool, std::vector<Model>&& models, bool usedForRayTracing) :
	models_(std::move(models))
{
	// Concatenate all the models
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Material> materials;
	std::vector<glm::vec4> procedurals;
	std::vector<std::pair<glm::vec3, glm::vec3>> aabbs;
	std::vector<glm::uvec2> offsets;

	for (const auto& model : models_)
	{
		// Remember the index, vertex offsets.
		const auto indexOffset = static_cast<uint32_t>(indices.size());
		const auto vertexOffset = static_cast<uint32_t>(vertices.size());
		const auto materialOffset = static_cast<uint32_t>(materials.size());

		offsets.emplace_back(indexOffset, vertexOffset);

		// Copy model data one after the other.
		vertices.insert(vertices.end(), model.Vertices().begin(), model.Vertices().end());
		indices.insert(indices.end(), model.Indices().begin(), model.Indices().end());
		materials.insert(materials.end(), model.Materials().begin(), model.Materials().end());

		// Adjust the material id.
		for (size_t i = vertexOffset; i != vertices.size(); ++i)
		{
			vertices[i].MaterialIndex += materialOffset;
		}

		// Add optional procedurals.
		const auto sphere = dynamic_cast<const Sphere*>(model.Procedural());
		if (sphere != nullptr)
		{
			aabbs.push_back(sphere->BoundingBox());
			procedurals.emplace_back(sphere->Center, sphere->Radius);
		}
		else
		{
			aabbs.emplace_back();
			procedurals.emplace_back();
		}
	}

	const auto flag = usedForRayTracing ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0;

	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | flag, vertices, vertexBuffer_, vertexBufferMemory_);
	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | flag, indices, indexBuffer_, indexBufferMemory_);
	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, materials, materialBuffer_, materialBufferMemory_);
	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, offsets, offsetBuffer_, offsetBufferMemory_);

	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, aabbs, aabbBuffer_, aabbBufferMemory_);
	CreateDeviceBuffer(commandPool, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, procedurals, proceduralBuffer_, proceduralBufferMemory_);

	//textureImage_.reset(new ::TextureImage(commandPool, "../assets/textures/chalet.jpg"));
}

Scene::~Scene()
{
	//textureImage_.reset();

	proceduralBuffer_.reset();
	proceduralBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	aabbBuffer_.reset();
	aabbBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	offsetBuffer_.reset();
	offsetBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	materialBuffer_.reset();
	materialBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	indexBuffer_.reset();
	indexBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	vertexBuffer_.reset();
	vertexBufferMemory_.reset(); // release memory after bound buffer has been destroyed
}

}
