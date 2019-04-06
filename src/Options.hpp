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

	// Scene options.
	uint32_t SceneIndex = 0;

	// Renderer options.
	uint32_t Samples = 8;
	uint32_t Bounces = 16;

	// Window options
	uint32_t Width{};
	uint32_t Height{};
	bool Fullscreen{};
	bool VSync{};
};
