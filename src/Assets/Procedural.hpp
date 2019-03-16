#pragma once

#include "Utilities/Glm.hpp"
#include <utility>

namespace Assets
{

	class Procedural
	{
	public:

		Procedural(const Procedural&) = delete;
		Procedural(Procedural&&) = delete;
		Procedural& operator = (const Procedural&) = delete;
		Procedural& operator = (Procedural&&) = delete;

		Procedural() = default;
		virtual ~Procedural() = default;;
		virtual std::pair<glm::vec3, glm::vec3> BoundingBox() const = 0;
	};
}
