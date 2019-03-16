#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan [0, 1] depth range, instead of OpenGL [-1, +1]
#define GLM_FORCE_RIGHT_HANDED // Vulkan has a left handed coordinate system (same as DirectX), OpenGL is right handed
#define GLM_FORCE_RADIANS
//#define GLM_FORCE_MESSAGES 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

