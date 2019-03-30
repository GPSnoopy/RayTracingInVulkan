#pragma once
#include "Utilities/Glm.hpp"
#include <functional>
#include <string>
#include <vector>

namespace Assets
{
	class Model;
}

class SceneList final
{
public:

	struct CameraInitialSate
	{
		glm::mat4 ModelView;
		float FieldOfView;
		float Aperture;
		float FocusDistance;
		bool GammaCorrection;
		bool HasSky;
	};

	static std::vector<Assets::Model> CubeAndSpheres(CameraInitialSate& camera);
	static std::vector<Assets::Model> RayTracingInOneWeekend(CameraInitialSate& camera);
	static std::vector<Assets::Model> LucyInOneWeekend(CameraInitialSate& camera);
	static std::vector<Assets::Model> CornellBox(CameraInitialSate& camera);
	static std::vector<Assets::Model> CornellBoxLucy(CameraInitialSate& camera);

	static const std::vector<std::pair<std::string, std::function<std::vector<Assets::Model>(CameraInitialSate&)>>> AllScenes;
};
