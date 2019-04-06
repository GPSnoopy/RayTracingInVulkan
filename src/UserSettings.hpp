#pragma once

struct UserSettings final
{
	// Application
	bool Benchmark;

	// Scene
	int SceneIndex;

	// Renderer
	bool IsRayTraced;
	bool AccumulateRays;
	uint32_t NumberOfSamples;
	uint32_t NumberOfBounces;

	// Camera
	float FieldOfView;
	float Aperture;
	float FocusDistance;
	bool GammaCorrection;

	// UI
	bool ShowSettings;
	bool ShowOverlay;

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
