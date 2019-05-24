# Ray Tracing In Vulkan

<img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/LucySettings.jpg">

My implementation of [Peter Shirley's Ray Tracing in One Weekend](https://github.com/petershirley/raytracinginoneweekend) books using Vulkan and NVIDIA's RTX extension (VK_NV_ray_tracing). This allows most scenes to be rendered at interactive speed on appropriate hardware.

The real-time ray tracer can also load full geometry from OBJ files as well as render the procedural spheres from the book. An accumulation buffer is used to increase the sample count when the camera is not moving while keeping the frame rate interactive. I have added a UI built using [Dear ImGui](https://github.com/ocornut/imgui) to allow changing the renderer parameters on the fly. Unlike projects such as [Q2VKPT](http://brechpunkt.de/q2vkpt/), there is no denoising filter. So the image will get noisy when moving the camera.

This personal project follows my own attempts at CPU ray tracing following Peter Shirley's books (see [here](https://github.com/GPSnoopy/RayTracingInOneWeekend) and [here](https://github.com/GPSnoopy/RayTracingTheNextWeek) if you are interested).

## Gallery

<img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/OneWeekend.jpg" width="33%"> <img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBox.jpg" width="33%"> <img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBoxLucy.jpg" width="33%">

## Performance

Using a GeForce RTX 2080 Ti, the rendering speed is obscenely faster than using the CPU renderer. Obviously both implementations are still quite naive in some places, but I'm really impressed by the performance. The cover scene of the first book reaches ~90fps at 1280x720 using 8 rays per pixel and up to 16 bounces.

I suspect performance could be improved further. I have created each object in the scene as a separate instance in the top level acceleration structure, which is probably not the best for data locality. The same goes for displaying multiple [Lucy statues](http://graphics.stanford.edu/data/3Dscanrep/), where I have naively duplicated the geometry rather than instancing it multiple times.

## Benchmarking

Command line arguments can be used to control various aspects of the application, including running it in benchmark mode. For mode information, use `--help`.

```
> RayTracer.exe --benchmark --width 2560 --height 1200 --fullscreen --scene 1
```
Here are my results with the command above on two different computers using NVIDIA drivers 425.31.

| Platform | Scene 1 | Scene 4 |
| --- | ---: | ---: |
| GeForce RTX 2080 Ti | 25.3 fps | 10.4 fps |
| GeForce GTX 1080 Ti | 3.6 fps | 1.3 fps |

## Building

Most of the third party dependencies can be built using [Microsoft's vcpkg](https://github.com/Microsoft/vcpkg).

**Windows (Visual Studio 2019 x64 solution)**
```
> vcpkg_windows.bat
> build_windows.bat
```
**Linux (GCC 8 Makefile)**
```
> sudo apt-get install curl unzip tar libxi-dev libxinerama-dev libxcursor-dev
> ./vcpkg_linux.sh
> ./build_linux.sh
```

Assuming you have installed the [Vulkan SDK](https://vulkan.lunarg.com/) and [CMake](https://cmake.org/), the build should work out-of-the box for Visual Studio. On Linux, you may have to manually install additional dependencies for `vcpkg_linux.sh` to succeed.

## References

* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Introduction to Real-Time Ray Tracing with Vulkan](https://devblogs.nvidia.com/vulkan-raytracing/)
* [NVIDIA Vulkan Ray Tracing Tutorial](https://developer.nvidia.com/rtx/raytracing/vkray)
* [NVIDIA Vulkan Ray Tracing Helpers: Introduction](https://developer.nvidia.com/rtx/raytracing/vkray_helpers)
* [Fast and Fun: My First Real-Time Ray Tracing Demo](https://devblogs.nvidia.com/my-first-ray-tracing-demo/)
* [Getting Started with RTX Ray Tracing](https://github.com/NVIDIAGameWorks/GettingStartedWithRTXRayTracing)
* [D3D12 Raytracing Samples](https://github.com/Microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing)
* [George Ouzounoudis's vk_exp](https://github.com/georgeouzou/vk_exp)
* [NVIDIA Vulkan Forums](https://devtalk.nvidia.com/default/board/166/vulkan/)
