#include "SceneList.hpp"
#include "Assets/Material.hpp"
#include "Assets/Model.hpp"
#include <functional>
#include <random>

using namespace glm;
using Assets::Material;
using Assets::Model;

const std::vector<std::pair<std::string, std::function<std::vector<Model>(SceneList::CameraInitialSate&)>>> SceneList::AllScenes =
{
	{"Cube And Spheres", CubeAndSpheres},
	{"Ray Tracing In One Weekend", RayTracingInOneWeekend},
	{"Lucy In One Weekend", LucyInOneWeekend},
	{"Cornell Box", CornellBox}
};

std::vector<Model> SceneList::CubeAndSpheres(CameraInitialSate& camera)
{
	camera.ModelView = translate(mat4(1), vec3(0, 0, -2));
	camera.FieldOfView = 90;
	camera.Aperture = 0.05f;
	camera.FocusDistance = 2.0f;
	camera.GammaCorrection = false;

	std::vector<Model> models;

	models.push_back(Model::LoadModel("../assets/models/cube_multi.obj"));
	models.push_back(Model::CreateSphere(vec3(1, 0, 0), 0.5, 3, Material::Metallic(vec3(0.7f, 0.5f, 0.8f), 0.2f), true));
	models.push_back(Model::CreateSphere(vec3(-1, 0, 0), 0.5, 3, Material::Dielectric(1.5f), true));

	return models;
}

std::vector<Model> SceneList::RayTracingInOneWeekend(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.1f;
	camera.FocusDistance = 10.0f;
	camera.GammaCorrection = true;

	const int subdiv = 3;
	const bool isProc = true;

	std::mt19937 engine(42);
	auto random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, subdiv + 2, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float chooseMat = random();
			const vec3 center(a + 0.9f*random(), 0.2f, b + 0.9f*random());

			if (length(center - vec3(4, 0.2f, 0)) > 0.9)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Lambertian(vec3(
						random()*random(),
						random()*random(),
						random()*random())),
						isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Metallic(
						vec3(0.5f*(1 + random()), 0.5f*(1 + random()), 0.5f*(1 + random())),
						0.5f*random()),
						isProc));
				}
				else // Glass
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	models.push_back(Model::CreateSphere(vec3(0, 1, 0), 1.0f, subdiv, Material::Dielectric(1.5f), isProc));
	models.push_back(Model::CreateSphere(vec3(-4, 1, 0), 1.0f, subdiv, Material::Lambertian(vec3(0.4f, 0.2f, 0.1f)), isProc));
	models.push_back(Model::CreateSphere(vec3(4, 1, 0), 1.0f, subdiv, Material::Metallic(vec3(0.7f, 0.6f, 0.5f), 0.0f), isProc));

	return models;
}

std::vector<Model> SceneList::LucyInOneWeekend(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 1.0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.05f;
	camera.FocusDistance = 10.0f;
	camera.GammaCorrection = true;

	const int subdiv = 3;
	const bool isProc = true;

	std::mt19937 engine(42);
	auto random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;
	
	models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, subdiv + 2, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float chooseMat = random();
			const vec3 center(a + 0.9f*random(), 0.2f, b + 0.9f*random());

			if (length(center - vec3(4, 0.2f, 0)) > 0.9)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Lambertian(vec3(
						random()*random(),
						random()*random(),
						random()*random())),
						isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Metallic(
						vec3(0.5f*(1 + random()), 0.5f*(1 + random()), 0.5f*(1 + random())),
						0.5f*random()),
						isProc));
				}
				else // Glass
				{
					models.push_back(Model::CreateSphere(center, 0.2f, subdiv, Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	auto lucy0 = Model::LoadModel("../assets/models/lucy.obj");
	auto lucy1 = lucy0;
	auto lucy2 = lucy0;

	const auto i = mat4(1);
	const float scaleFactor = 0.0035f;

	lucy0.Transform(
		rotate(
			scale(
				translate(i, vec3(0, -0.08f, 0)), 
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy1.Transform(
		rotate(
			scale(
				translate(i, vec3(-4, -0.08f, 0)),
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy2.Transform(
		rotate(
			scale(
				translate(i, vec3(4, -0.08f, 0)),
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy0.SetMaterial(Material::Dielectric(1.5f));
	lucy1.SetMaterial(Material::Lambertian(vec3(0.4f, 0.2f, 0.1f)));
	lucy2.SetMaterial(Material::Metallic(vec3(0.7f, 0.6f, 0.5f), 0.05f));

	models.push_back(std::move(lucy0));
	models.push_back(std::move(lucy1));
	models.push_back(std::move(lucy2));

	return models;
}

std::vector<Assets::Model> SceneList::CornellBox(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(278, 278, 800), vec3(278, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.GammaCorrection = true;
	//sky = false;

	std::vector<Model> models;

	models.push_back(Model::CreateCornellBox());

	return models;
}
