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

	uint32_t Width{};
	uint32_t Height{};
	bool Fullscreen{};
	bool VSync{};
};
