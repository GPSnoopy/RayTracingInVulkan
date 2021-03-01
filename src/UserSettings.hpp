#pragma once

#include <array>

struct UserSettings final
{
	// Application
	bool Benchmark;

	// Benchmark
	bool BenchmarkNextScenes{};
	uint32_t BenchmarkMaxTime{};
	
	// Scene
	int SceneIndex;

	// Renderer
	bool IsRayTraced;
	bool AccumulateRays;
	bool DefaultBounces;
	uint32_t NumberOfSamples;
	uint32_t NumberOfBounces;
	uint32_t MaxNumberOfSamples;

	// Camera
	float FieldOfView;
	float Aperture;
	float FocusDistance;

	// Profiler
	bool ShowHeatmap;
	float HeatmapScale;

	// UI
	bool ShowSettings;
	bool ShowOverlay;

	inline const static float FieldOfViewMinValue = 10.0f;
	inline const static float FieldOfViewMaxValue = 90.0f;
	
	const int NumberOfDefaultBounces(unsigned int SceneIndex)
	{
		std::array<int,4> ArrayOfDefaultBounces= { 8, 8, 8, 16 };
		if (SceneIndex >= ArrayOfDefaultBounces.size())
			return 8;
		return ArrayOfDefaultBounces.at(SceneIndex);
	};

	bool RequiresAccumulationReset(const UserSettings& prev) const
	{
		return
			IsRayTraced != prev.IsRayTraced ||
			AccumulateRays != prev.AccumulateRays ||
			NumberOfBounces != prev.NumberOfBounces ||
			FieldOfView != prev.FieldOfView ||
			Aperture != prev.Aperture ||
			FocusDistance != prev.FocusDistance;
	}
};
