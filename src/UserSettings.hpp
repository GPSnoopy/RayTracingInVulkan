#pragma once

struct UserSettings final
{
	// Scene
	int SceneIndex = 0;

	// Renderer
	bool IsRayTraced = false;
	bool AccumulateRays = true;
	uint32_t NumberOfSamples = 8;
	uint32_t NumberOfBounces = 16;

	// Camera
	float FieldOfView = 45.0f;
	float Aperture = 0.0f;
	float FocusDistance = 1.0f;
	bool GammaCorrection = true;

	// UI
	bool ShowSettings = true;
	bool ShowOverlay = true;

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
