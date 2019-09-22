#pragma once

#include "Utilities/Glm.hpp"

namespace Assets
{

	struct alignas(16) Material final
	{
		static Material Lambertian(const glm::vec3& diffuse, const int32_t textureId = -1)
		{
			return Material{ glm::vec4(diffuse, 1), textureId, 0.0f, 0.0f, Enum::Lambertian };
		}

		static Material Metallic(const glm::vec3& diffuse, const float fuzziness, const int32_t textureId = -1)
		{
			return Material{ glm::vec4(diffuse, 1), textureId, fuzziness, 0.0f, Enum::Metallic };
		}

		static Material Dielectric(const float refractionIndex, const int32_t textureId = -1)
		{
			return Material{ glm::vec4(0.7f, 0.7f, 1.0f, 1), textureId,  0.0f, refractionIndex, Enum::Dielectric };
		}

		static Material Isotropic(const glm::vec3& diffuse, const int32_t textureId = -1)
		{
			return Material{ glm::vec4(diffuse, 1), textureId, 0.0f, 0.0f, Enum::Isotropic };
		}

		static Material DiffuseLight(const glm::vec3& diffuse, const int32_t textureId = -1)
		{
			return Material{ glm::vec4(diffuse, 1), textureId, 0.0f, 0.0f, Enum::DiffuseLight };
		}

		enum class Enum : uint32_t
		{
			Lambertian = 0,
			Metallic = 1,
			Dielectric = 2,
			Isotropic = 3,
			DiffuseLight = 4
		};

		// Note: vec3 and vec4 gets aligned on 16 bytes in Vulkan shaders. 

		// Base material
		glm::vec4 Diffuse;
		int32_t DiffuseTextureId;

		// Metal fuzziness
		float Fuzziness;

		// Dielectric refraction index
		float RefractionIndex;

		// Which material are we dealing with
		Enum MaterialModel;
	};

}