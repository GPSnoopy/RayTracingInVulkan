#include "Console.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Utilities {

int Console::SetColorBySeverity(const Severity severity) noexcept
{
#ifdef WIN32
	const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO info = {};
	GetConsoleScreenBufferInfo(hConsole, &info);

	switch (severity)
	{
	case Severity::Verbos:
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		break;
	case Severity::Info:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case Severity::Warning:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case Severity::Error:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case Severity::Fatal:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	default:;
	}

	return info.wAttributes;
#else
	return 0;
#endif
}

void Console::SetColorByAttributes(const int attributes) noexcept
{
#ifdef WIN32
	const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	SetConsoleTextAttribute(hConsole, attributes);
#endif
}

}
