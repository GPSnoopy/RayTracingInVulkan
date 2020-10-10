#include "Options.hpp"
#include "SceneList.hpp"
#include "Utilities/Exception.hpp"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

Options::Options(const int argc, const char* argv[])
{
	const int lineLength = 120;
	
	options_description benchmark("Benchmark options", lineLength);
	benchmark.add_options()
		("next-scenes", bool_switch(&BenchmarkNextScenes)->default_value(false), "Load the next scene once the sample or time limit is reached.")
		("max-time", value<uint32_t>(&BenchmarkMaxTime)->default_value(60), "The benchmark time limit per scene (in seconds).")
		;

	options_description renderer("Renderer options", lineLength);
	renderer.add_options()
		("samples", value<uint32_t>(&Samples)->default_value(8), "The number of ray samples per pixel.")
		("bounces", value<uint32_t>(&Bounces)->default_value(16), "The maximum number of bounces per ray.")
		("max-samples", value<uint32_t>(&MaxSamples)->default_value(64 * 1024), "The maximum number of accumulated ray samples per pixel.")
		;

	options_description scene("Scene options", lineLength);
	scene.add_options()
		("scene", value<uint32_t>(&SceneIndex)->default_value(1), "The scene to start with.")
		;

	options_description window("Window options", lineLength);
	window.add_options()
		("width", value<uint32_t>(&Width)->default_value(1280), "The framebuffer width.")
		("height", value<uint32_t>(&Height)->default_value(720), "The framebuffer height.")
		("present-mode", value<uint32_t>(&PresentMode)->default_value(2), "The present mode (0 = Immediate, 1 = MailBox, 2 = FIFO, 3 = FIFORelaxed).")
		("fullscreen", bool_switch(&Fullscreen)->default_value(false), "Toggle fullscreen vs windowed (default: windowed).")
		;

	options_description desc("Application options", lineLength);
	desc.add_options()
		("help", "Display help message.")
		("benchmark", bool_switch(&Benchmark)->default_value(false), "Run the application in benchmark mode.")
		;

	desc.add(benchmark);
	desc.add(renderer);
	desc.add(scene);
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

	if (PresentMode > 3)
	{
		Throw(std::out_of_range("invalid present mode"));
	}
}

