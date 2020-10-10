#pragma once

#include <cstdint>
#include <exception>

class Options final
{
public:

	class Help : public std::exception
	{
	public:

		Help() = default;
		~Help() = default;
	};

	Options(int argc, const char* argv[]);
	~Options() = default;

	// Application options.
	bool Benchmark{};
	// 
	// Benchmark options.
	bool BenchmarkNextScenes{};
	uint32_t BenchmarkMaxTime{};

	// Scene options.
	uint32_t SceneIndex{};

	// Renderer options.
	uint32_t Samples{};
	uint32_t Bounces{};
	uint32_t MaxSamples{};

	// Window options
	uint32_t Width{};
	uint32_t Height{};
	uint32_t PresentMode{};
	bool Fullscreen{};
};
