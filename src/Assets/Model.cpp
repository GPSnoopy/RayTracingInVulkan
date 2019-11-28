#include "Model.hpp"
#include "CornellBox.hpp"
#include "Procedural.hpp"
#include "Sphere.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/Console.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace glm;

namespace std
{
	template<> struct hash<Assets::Vertex> final
	{
		size_t operator()(Assets::Vertex const& vertex) const noexcept
		{
			return
				Combine(hash<vec3>()(vertex.Position),
					Combine(hash<vec3>()(vertex.Normal),
						Combine(hash<vec2>()(vertex.TexCoord),
							hash<int>()(vertex.MaterialIndex))));
		}

	private:

		static size_t Combine(size_t hash0, size_t hash1)
		{
			return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
		}
	};
}

namespace Assets {

Model Model::LoadModel(const std::string& filename)
{
	std::cout << "Loading '" << filename << "'... " << std::flush;

	const auto timer = std::chrono::high_resolution_clock::now();
	const std::string materialPath = std::filesystem::path(filename).parent_path().string();
	
	tinyobj::attrib_t objAttrib;
	std::vector<tinyobj::shape_t> objShapes;
	std::vector<tinyobj::material_t> objMaterials;
	std::string warn;
	std::string err;

	if (!tinyobj::LoadObj(
		&objAttrib, &objShapes, &objMaterials, &warn, &err,
		filename.c_str(),
		materialPath.c_str()))
	{
		Throw(std::runtime_error("failed to load model '" + filename + "':\n" + err));
	}

	if (!warn.empty())
	{
		Utilities::Console::Write(Utilities::Severity::Warning, [&warn]()
		{
			std::cout << "\nWARNING: " << warn << std::flush;
		});
	}

	// Materials
	std::vector<Material> materials;

	for (const auto& material : objMaterials)
	{
		Material m{};

		m.Diffuse = vec4(material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0);
		m.DiffuseTextureId = -1;

		materials.emplace_back(m);
	}

	if (materials.empty())
	{
		Material m{};

		m.Diffuse = vec4(0.7f, 0.7f, 0.7f, 1.0);
		m.DiffuseTextureId = -1;

		materials.emplace_back(m);
	}

	// Geometry
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<Vertex, uint32_t> uniqueVertices(objAttrib.vertices.size());
	size_t faceId = 0;

	for (const auto& shape : objShapes)
	{
		const auto& mesh = shape.mesh;

		for (const auto& index : mesh.indices)
		{
			Vertex vertex = {};

			vertex.Position =
			{
				objAttrib.vertices[3 * index.vertex_index + 0],
				objAttrib.vertices[3 * index.vertex_index + 1],
				objAttrib.vertices[3 * index.vertex_index + 2],
			};

			vertex.Normal =
			{
				objAttrib.normals[3 * index.normal_index + 0],
				objAttrib.normals[3 * index.normal_index + 1],
				objAttrib.normals[3 * index.normal_index + 2]
			};

			if (!objAttrib.texcoords.empty())
			{
				vertex.TexCoord =
				{
					objAttrib.texcoords[2 * index.texcoord_index + 0],
					1 - objAttrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			vertex.MaterialIndex = std::max(0, mesh.material_ids[faceId++ / 3]);

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	const auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();

	std::cout << "(" << objAttrib.vertices.size() << " vertices, " << uniqueVertices.size() << " unique vertices, " << materials.size() << " materials) ";
	std::cout << elapsed << "s" << std::endl;

	return Model(std::move(vertices), std::move(indices), std::move(materials), nullptr);
}

Model Model::CreateCornellBox(const float scale)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Material> materials;

	CornellBox::Create(scale, vertices, indices, materials);

	return Model(
		std::move(vertices),
		std::move(indices),
		std::move(materials),
		nullptr
	);
}

Model Model::CreateBox(const vec3& p0, const vec3& p1, const Material& material)
{
	std::vector<Vertex> vertices = 
	{
		Vertex{vec3(p0.x, p0.y, p0.z), vec3(-1, 0, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p0.y, p1.z), vec3(-1, 0, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p1.z), vec3(-1, 0, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p0.z), vec3(-1, 0, 0), vec2(0), 0},

		Vertex{vec3(p1.x, p0.y, p1.z), vec3(1, 0, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p0.y, p0.z), vec3(1, 0, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p1.y, p0.z), vec3(1, 0, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p1.y, p1.z), vec3(1, 0, 0), vec2(0), 0},

		Vertex{vec3(p1.x, p0.y, p0.z), vec3(0, 0, -1), vec2(0), 0},
		Vertex{vec3(p0.x, p0.y, p0.z), vec3(0, 0, -1), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p0.z), vec3(0, 0, -1), vec2(0), 0},
		Vertex{vec3(p1.x, p1.y, p0.z), vec3(0, 0, -1), vec2(0), 0},

		Vertex{vec3(p0.x, p0.y, p1.z), vec3(0, 0, 1), vec2(0), 0},
		Vertex{vec3(p1.x, p0.y, p1.z), vec3(0, 0, 1), vec2(0), 0},
		Vertex{vec3(p1.x, p1.y, p1.z), vec3(0, 0, 1), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p1.z), vec3(0, 0, 1), vec2(0), 0},

		Vertex{vec3(p0.x, p0.y, p0.z), vec3(0, -1, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p0.y, p0.z), vec3(0, -1, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p0.y, p1.z), vec3(0, -1, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p0.y, p1.z), vec3(0, -1, 0), vec2(0), 0},

		Vertex{vec3(p1.x, p1.y, p0.z), vec3(0, 1, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p0.z), vec3(0, 1, 0), vec2(0), 0},
		Vertex{vec3(p0.x, p1.y, p1.z), vec3(0, 1, 0), vec2(0), 0},
		Vertex{vec3(p1.x, p1.y, p1.z), vec3(0, 1, 0), vec2(0), 0},
	};

	std::vector<uint32_t> indices =
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	return Model(
		std::move(vertices),
		std::move(indices),
		std::vector<Material>{material},
		nullptr);
}

Model Model::CreateSphere(const vec3& center, float radius, const Material& material, const bool isProcedural)
{
	const int slices = 32;
	const int stacks = 16;
	
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const float pi = 3.14159265358979f;
	
	for (int j = 0; j <= stacks; ++j) 
	{
		const float j0 = pi * j / stacks;

		// Vertex
		const float v = radius * -std::sin(j0);
		const float z = radius * std::cos(j0);
		
		// Normals		
		const float n0 = -std::sin(j0);
		const float n1 = std::cos(j0);

		for (int i = 0; i <= slices; ++i) 
		{
			const float i0 = 2 * pi * i / slices;

			const vec3 position(
				center.x + v * std::sin(i0),
				center.y + z,
				center.z + v * std::cos(i0));
			
			const vec3 normal(
				n0 * std::sin(i0),
				n1,
				n0 * std::cos(i0));

			const vec2 texCoord(
				static_cast<float>(i) / slices,
				static_cast<float>(j) / stacks);

			vertices.push_back(Vertex{ position, normal, texCoord, 0 });
		}
	}

	for (int j = 0; j < stacks; ++j)
	{
		for (int i = 0; i < slices; ++i)
		{
			const auto j0 = (j + 0) * (slices + 1);
			const auto j1 = (j + 1) * (slices + 1);
			const auto i0 = i + 0;
			const auto i1 = i + 1;
			
			indices.push_back(j0 + i0);
			indices.push_back(j1 + i0);
			indices.push_back(j1 + i1);
			
			indices.push_back(j0 + i0);
			indices.push_back(j1 + i1);
			indices.push_back(j0 + i1);
		}
	}

	return Model(
		std::move(vertices),
		std::move(indices),
		std::vector<Material>{material},
		isProcedural ? new Sphere(center, radius) : nullptr);
}

void Model::ClearMaterials()
{
	size_t size = materials_.size();
	materials_.clear();
	for (size_t i = 0; i < size; i++) {
		materials_.push_back(Material());
	}
}

void Model::SetMaterial(const Material& material)
{
	if (materials_.size() != 1)
	{
		Throw(std::runtime_error("cannot change material on a multi-material model"));
	}

	materials_[0] = material;
}

void Model::SetMaterials(std::vector<Material> materials)
{
	if (materials.size() != materials_.size())
	{
		Throw(std::runtime_error("cannot alter number of materials"));
	}

	materials_ = materials;
}

void Model::Transform(const mat4& transform)
{
	const auto transformIT = inverseTranspose(transform);

	for (auto& vertex : vertices_)
	{
		vertex.Position = transform * vec4(vertex.Position, 1);
		vertex.Normal = transformIT * vec4(vertex.Normal, 0);
	}
}

Model::Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural) :
	vertices_(std::move(vertices)), 
	indices_(std::move(indices)),
	materials_(std::move(materials)),
	procedural_(procedural)
{
}

}
