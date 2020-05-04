#pragma once

#include "Utilities/Glm.hpp"

class Camera final
{
public:

	void Reset(const glm::mat4& modelView);

	glm::mat4 ModelView() const;

	void MoveForward(float d);
	void MoveRight(float d);
	void MoveUp(float d);
	void Rotate(float y, float x);

private:

	void UpdateVectors();

	glm::mat4 orientation_{};

	glm::vec4 position_{};
	glm::vec4 right_{ 1, 0, 0, 0 };
	glm::vec4 up_{ 0, 1, 0, 0 };
	glm::vec4 forward_{ 0, 0, -1, 0 };
	
};
