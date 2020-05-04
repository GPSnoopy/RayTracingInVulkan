#include "Camera.hpp"

void Camera::Reset(const glm::mat4& modelView)
{
	const auto inverse = glm::inverse(modelView);

	position_ = inverse * glm::vec4(0, 0, 0, 1);
	orientation_ = glm::mat4(glm::mat3(modelView));

	UpdateVectors();
}

glm::mat4 Camera::ModelView() const
{
	return orientation_ * glm::translate(glm::mat4(1), -glm::vec3(position_));
}

void Camera::MoveForward(const float d)
{
	position_ += d * forward_;
}

void Camera::MoveRight(float d)
{
	position_ += d * right_;
}

void Camera::MoveUp(float d)
{
	position_ += d * up_;
}

void Camera::Rotate(float y, float x)
{
	orientation_ =
		glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0)) *
		orientation_ *
		glm::rotate(glm::mat4(1), y, glm::vec3(0, 1, 0));

	UpdateVectors();
}

void Camera::UpdateVectors()
{
	const auto inverse = glm::inverse(orientation_);
	
	right_ = inverse * glm::vec4(1, 0, 0, 0);
	up_ = inverse * glm::vec4(0, 1, 0, 0);
	forward_ = inverse * glm::vec4(0, 0, -1, 0);
}
