#pragma once

#include "Utilities/Glm.hpp"

namespace Assets
{

	struct alignas(16) Material final
	{
		static Material Lambertian(const glm::vec3& diffuse)
		{
			return Material{ glm::vec4(diffuse, 1), 0.0f, 0.0f, Enum::Lambertian };
		}

		static Material Metalic(const glm::vec3& diffuse, const float fuzziness)
		{
			return Material{ glm::vec4(diffuse, 1), fuzziness, 0.0f, Enum::Metalic };
		}

		static Material Dielectric(const float refractionIndex)
		{
			return Material{ glm::vec4(0.7f, 0.7f, 1.0f, 1), 0.0f, refractionIndex, Enum::Dielectric };
		}

		enum class Enum : uint32_t
		{
			Lambertian = 0,
			Metalic = 1,
			Dielectric = 2
		};

		// Note: vec3 and vec4 gets aligned on 16 bytes in Vulkan shaders. 

		// Base material
		glm::vec4 Diffuse;
		//int DiffuseTexture; // TODO Make it the A component

		// Metal fuzziness
		float Fuzziness;

		// Dielectric refraction index
		float RefractionIndex;

		// Which material are we dealing with
		Enum MaterialModel;
	};

}