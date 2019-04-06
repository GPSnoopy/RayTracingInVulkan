#include "Options.hpp"
#include "SceneList.hpp"
#include "Utilities/Exception.hpp"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

Options::Options(const int argc, const char* argv[])
{


	options_description scene("Scene options");
	scene.add_options()
		("scene", value<uint32_t>(&SceneIndex)->default_value(1), "Set the scene to start with.")
		;

	options_description renderer("Renderer options");
	renderer.add_options()
		("samples", value<uint32_t>(&Samples)->default_value(8), "Set the number of ray samples per pixel.")
		("bounces", value<uint32_t>(&Bounces)->default_value(16), "Set the maximum number of bounces per ray.")
		;

	options_description window("Window options");
	window.add_options()
		("width", value<uint32_t>(&Width)->default_value(1280), "Set framebuffer width.")
		("height", value<uint32_t>(&Height)->default_value(720), "Set framebuffer height.")
		("fullscreen", bool_switch(&Fullscreen)->default_value(false), "Toggle fullscreen vs windowed (default: windowed).")
		("vsync", bool_switch(&VSync)->default_value(false), "Toggle vsync (default: vsync off).")
		;

	options_description desc("Application options");
	desc.add_options()
		("help", "Display help message.")
		("benchmark", bool_switch(&Benchmark)->default_value(false), "Run the application in benchmark mode")
		;

	desc.add(scene);
	desc.add(renderer);
	desc.add(window);

	const positional_options_description positional;
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(positional).run(), vm);
	notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		Throw(Help());
	}

	if (SceneIndex >= SceneList::AllScenes.size())
	{
		Throw(std::out_of_range("scene index is too large"));
	}
}

