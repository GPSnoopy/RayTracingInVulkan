#include "ModelViewController.hpp"
#include "Vulkan/Vulkan.hpp"

void ModelViewController::Reset(const glm::mat4& modelView)
{
	const auto inverse = glm::inverse(modelView);

	position_ = inverse * glm::vec4(0, 0, 0, 1);
	orientation_ = glm::mat4(glm::mat3(modelView));
	
	cameraRotX_ = 0;
	cameraRotY_ = 0;
	modelRotX_ = 0;
	modelRotY_ = 0;

	mouseLeftPressed_ = false;
	mouseRightPressed_ = false;

	UpdateVectors();
}

glm::mat4 ModelViewController::ModelView() const
{
	const auto cameraRotX = static_cast<float>(modelRotY_ / 300.0);
	const auto cameraRotY = static_cast<float>(modelRotX_ / 300.0);
	
	const auto model =
		glm::rotate(glm::mat4(1.0f), cameraRotY * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), cameraRotX * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	const auto view = orientation_ * glm::translate(glm::mat4(1), -glm::vec3(position_));

	return view * model;
}

bool ModelViewController::OnKey(const int key, const int scancode, const int action, const int mods)
{
	switch (key)
	{
	case GLFW_KEY_S: cameraMovingBackward_ = action != GLFW_RELEASE; return true;
	case GLFW_KEY_W: cameraMovingForward_ = action != GLFW_RELEASE; return true;
	case GLFW_KEY_A: cameraMovingLeft_ = action != GLFW_RELEASE; return true;
	case GLFW_KEY_D: cameraMovingRight_ = action != GLFW_RELEASE; return true;
	case GLFW_KEY_LEFT_CONTROL: cameraMovingDown_ = action != GLFW_RELEASE; return true;
	case GLFW_KEY_LEFT_SHIFT: cameraMovingUp_ = action != GLFW_RELEASE; return true;
	default: return false;
	}
}

bool ModelViewController::OnCursorPosition(const double xpos, const double ypos)
{
	const auto deltaX = static_cast<float>(xpos - mousePosX_);
	const auto deltaY = static_cast<float>(ypos - mousePosY_);

	if (mouseLeftPressed_)
	{
		cameraRotX_ += deltaX;
		cameraRotY_ += deltaY;
	}

	if (mouseRightPressed_)
	{
		modelRotX_ += deltaX;
		modelRotY_ += deltaY;
	}

	mousePosX_ = xpos;
	mousePosY_ = ypos;

	return mouseLeftPressed_ || mouseRightPressed_;
}

bool ModelViewController::OnMouseButton(const int button, const int action, const int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		mouseLeftPressed_ = action == GLFW_PRESS;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		mouseRightPressed_ = action == GLFW_PRESS;
	}

	return true;
}

bool ModelViewController::UpdateCamera(const double speed, const double timeDelta)
{
	const auto d = static_cast<float>(speed * timeDelta);

	if (cameraMovingLeft_) MoveRight(-d);
	if (cameraMovingRight_) MoveRight(d);
	if (cameraMovingBackward_) MoveForward(-d);
	if (cameraMovingForward_) MoveForward(d);
	if (cameraMovingDown_) MoveUp(-d);
	if (cameraMovingUp_) MoveUp(d);

	const float rotationDiv = 300;
	Rotate(cameraRotX_ / rotationDiv, cameraRotY_ / rotationDiv);

	const bool updated =
		cameraMovingLeft_ ||
		cameraMovingRight_ ||
		cameraMovingBackward_ ||
		cameraMovingForward_ ||
		cameraMovingDown_ ||
		cameraMovingUp_ ||
		cameraRotY_ != 0 ||
		cameraRotX_ != 0;

	cameraRotY_ = 0;
	cameraRotX_ = 0;

	return updated;
}

void ModelViewController::MoveForward(const float d)
{
	position_ += d * forward_;
}

void ModelViewController::MoveRight(const float d)
{
	position_ += d * right_;
}

void ModelViewController::MoveUp(const float d)
{
	position_ += d * up_;
}

void ModelViewController::Rotate(const float y, const float x)
{
	orientation_ =
		glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0)) *
		orientation_ *
		glm::rotate(glm::mat4(1), y, glm::vec3(0, 1, 0));

	UpdateVectors();
}

void ModelViewController::UpdateVectors()
{
	// Given the ortientation matrix, find out the x,y,z vector orientation.
	const auto inverse = glm::inverse(orientation_);
	
	right_ = inverse * glm::vec4(1, 0, 0, 0);
	up_ = inverse * glm::vec4(0, 1, 0, 0);
	forward_ = inverse * glm::vec4(0, 0, -1, 0);
}
