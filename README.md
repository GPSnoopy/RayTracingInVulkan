# Ray Tracing In Vulkan

<img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/LucySettings.jpg">

My implementation of [Peter Shirley's Ray Tracing in One Weekend](https://github.com/RayTracing/raytracing.github.io) books using Vulkan and NVIDIA's RTX extension (VK_NV_ray_tracing, now ported to NV_KHR_ray_tracing). This allows most scenes to be rendered at interactive speed on appropriate hardware.

The real-time ray tracer can also load full geometry from OBJ files as well as render the procedural spheres from the book. An accumulation buffer is used to increase the sample count when the camera is not moving while keeping the frame rate interactive. I have added a UI built using [Dear ImGui](https://github.com/ocornut/imgui) to allow changing the renderer parameters on the fly. Unlike projects such as [Q2VKPT](http://brechpunkt.de/q2vkpt/), there is no denoising filter. So the image will get noisy when moving the camera.

This personal project follows my own attempts at CPU ray tracing following Peter Shirley's books (see [here](https://github.com/GPSnoopy/RayTracingInOneWeekend) and [here](https://github.com/GPSnoopy/RayTracingTheNextWeek) if you are interested).

## Gallery

<img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/OneWeekend.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/Planets.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBox.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBoxLucy.jpg" width="49%"></img>

## Performance

Using a GeForce RTX 2080 Ti, the rendering speed is obscenely faster than using the CPU renderer. Obviously both implementations are still quite naive in some places, but I'm really impressed by the performance. The cover scene of the first book reaches ~140fps at 1280x720 using 8 rays per pixel and up to 16 bounces.

I suspect performance could be improved further. I have created each object in the scene as a separate instance in the top level acceleration structure, which is probably not the best for data locality. The same goes for displaying multiple [Lucy statues](http://graphics.stanford.edu/data/3Dscanrep/), where I have naively duplicated the geometry rather than instancing it multiple times.

## Benchmarking

Command line arguments can be used to control various aspects of the application. Use `--help` to see all modes and arguments. For example, to run the ray tracer in benchmark mode in 2560x1440 fullscreen for scene #1:
```
> RayTracer.exe --benchmark --width 2560 --height 1440 --fullscreen --scene 1
```
To benchmark all the scenes, starting from scene #1:
```
> RayTracer.exe --benchmark --width 2560 --height 1440 --fullscreen --scene 1 --next-scenes
```
Here are my results with the command above on a few different computers using NVIDIA drivers 436.48 and Release 4 of the RayTracer.

| Platform | Scene 1 | Scene 2 | Scene 3 | Scene 4 | Scene 5 |
| --- | ---: | ---: | ---: | ---: | ---: |
| GeForce RTX 2080 Ti | 36.1 fps | 35.7 fps | 19.9 fps | 54.9 fps | 15.1 fps |
| GeForce RTX 2070 | 19.9 fps | 19.9 fps | 11.7 fps | 30.4 fps | 9.5 fps |
| GeForce GTX 1080 Ti | 3.4 fps | 3.4 fps | 1.9 fps | 3.8 fps | 1.3 fps |

## Building

First you will need to install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). For Windows, LunarG provides installers. For Ubuntu LTS, they have native packages available. For other Linux distributions, they only provide tarballs. The rest of the third party dependencies can be built using [Microsoft's vcpkg](https://github.com/Microsoft/vcpkg) as provided by the scripts below.

If in doubt, please check the GitHub Actions [continuous integration configurations](.github/workflows) for more details.

**Windows (Visual Studio 2019 x64 solution)** [![Windows CI Status](https://github.com/GPSnoopy/RayTracingInVulkan/workflows/Windows%20CI/badge.svg)](https://github.com/GPSnoopy/RayTracingInVulkan/actions?query=workflow%3A%22Windows+CI%22)
```
> vcpkg_windows.bat
> build_windows.bat
```
**Linux (GCC 8 Makefile)** [![Linux CI Status](https://github.com/GPSnoopy/RayTracingInVulkan/workflows/Linux%20CI/badge.svg)](https://github.com/GPSnoopy/RayTracingInVulkan/actions?query=workflow%3A%22Linux+CI%22)
```
> sudo apt-get install curl unzip tar libxi-dev libxinerama-dev libxcursor-dev
> ./vcpkg_linux.sh
> ./build_linux.sh
```

## Random Thoughts

- I suspect the RTX 20 series RT cores to implement ray-AABB collision detection using reduced float precision. Early in the development, when trying to get the sphere procedural rendering to work, reporting an intersection every time the `rint` shader is invoked allowed to visualise the AABB of each procedural instance. The rendering of the bounding volume had many artifacts around the boxes edges, typical of reduced precision.

- When I upgraded the drivers to 430.86, performance significantly improved (+50%). This was around the same time Quake II RTX was released by NVIDIA. Coincidence?

- When looking at the benchmark results of an RTX 2070 and an RTX 2080 Ti, the performance differences mostly in line with the number of CUDA cores and RT cores rather than being influences by other metrics. Although I do not know at this point whether the CUDA cores or the RT cores are the main bottleneck.

- The benchmark mode makes the Boost clock behaviour quite obvious. Performance degrades rapidly by a few % over 60-120 seconds. For example, in theory the RTX 2080 Ti should be getting exactly the same result for Scene 1 and Scene 2 (or very close). Yet because Scene 2 is run after, the GPU has had to reduce its Boost clock as the GPU gets close to its thermal limit.


## References

* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Introduction to Real-Time Ray Tracing with Vulkan](https://devblogs.nvidia.com/vulkan-raytracing)
* [NVIDIA Vulkan Ray Tracing Tutorial](https://developer.nvidia.com/rtx/raytracing/vkray)
* [NVIDIA Vulkan Ray Tracing Helpers: Introduction](https://developer.nvidia.com/rtx/raytracing/vkray_helpers)
* [NVIDIA Vulkan Ray Tracing Tutorial (VK_KHR_ray_tracing)](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR)
* [NVIDIA Converting VK_NV_ray_tracing to VK_KHR_ray_tracing](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/NV_to_KHR.md.htm)
* [Fast and Fun: My First Real-Time Ray Tracing Demo](https://devblogs.nvidia.com/my-first-ray-tracing-demo/)
* [Getting Started with RTX Ray Tracing](https://github.com/NVIDIAGameWorks/GettingStartedWithRTXRayTracing)
* [D3D12 Raytracing Samples](https://github.com/Microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing)
* [George Ouzounoudis's vk_exp](https://github.com/georgeouzou/vk_exp)
* [NVIDIA Vulkan Forums](https://devtalk.nvidia.com/default/board/166/vulkan)

