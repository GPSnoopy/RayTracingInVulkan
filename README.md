# Towards Real-Time Simulation of Human Skin Appearance

Implemented by Ruth Holness

## Gallery

<img align="center" src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/LucySettings.jpg">
<img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/OneWeekend.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/Planets.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBox.jpg" width="49%"></img> <img src="https://github.com/GPSnoopy/RayTracingInVulkan/blob/master/gallery/CornellBoxLucy.jpg" width="49%"></img>

## Performance

Using a GeForce RTX 2080 Ti, the rendering speed is obscenely faster than using the CPU renderer. Obviously both implementations are still quite naive in some places, but they're impressed by the performance. The cover scene of the first book reaches ~140fps at 1280x720 using 8 rays per pixel and up to 16 bounces.

They suspect performance could be improved further. They have created each object in the scene as a separate instance in the top level acceleration structure, which is probably not the best for data locality. The same goes for displaying multiple [Lucy statues](http://graphics.stanford.edu/data/3Dscanrep/), where they have naively duplicated the geometry rather than instancing it multiple times.

## Benchmarking

Command line arguments can be used to control various aspects of the application. Use `--help` to see all modes and arguments. For example, to run the ray tracer in benchmark mode in 2560x1440 fullscreen for scene #1:
```
> RayTracer.exe --benchmark --width 2560 --height 1440 --fullscreen --scene 1
```
To benchmark all the scenes, starting from scene #1:
```
> RayTracer.exe --benchmark --width 2560 --height 1440 --fullscreen --scene 1 --next-scenes
```
Here are the results with the command above on a few different computers using NVIDIA drivers 436.48 and Release 4 of the RayTracer.

| Platform | Scene 1 | Scene 2 | Scene 3 | Scene 4 | Scene 5 |
| --- | ---: | ---: | ---: | ---: | ---: |
| GeForce RTX 2080 Ti | 36.1 fps | 35.7 fps | 19.9 fps | 54.9 fps | 15.1 fps |
| GeForce RTX 2070 | 19.9 fps | 19.9 fps | 11.7 fps | 30.4 fps | 9.5 fps |
| GeForce GTX 1080 Ti | 3.4 fps | 3.4 fps | 1.9 fps | 3.8 fps | 1.3 fps |

## Building

Most of the third party dependencies can be built using [Microsoft's vcpkg](https://github.com/Microsoft/vcpkg).

**Windows (Visual Studio 2019 x64 solution)** [![Windows CI Status](https://github.com/GPSnoopy/RayTracingInVulkan/workflows/Windows%20CI/badge.svg)](https://github.com/GPSnoopy/RayTracingInVulkan/actions?query=workflow%3A%22Windows+CI%22)
```
> vcpkg_windows.bat
> build_windows.bat
```
vcpkg_windows.bat fails on H drive (why, I don't know) => need to run on C drive and copy over.
build_windows.bat fails first time as it gives the error 
```Unable to open check cache file for write. .../RayTracingInVulkan/build/windows/CMakeFiles/CMakeTmp/CMakeFiles/cmake.check_cache```
Creating a file with that name in that location (can copy the one from ```.../RayTracingInVulkan/build/windows/CMakeFiles/```) fixes this and it will build.

**Linux (GCC 8 Makefile)** [![Linux CI Status](https://github.com/GPSnoopy/RayTracingInVulkan/workflows/Linux%20CI/badge.svg)](https://github.com/GPSnoopy/RayTracingInVulkan/actions?query=workflow%3A%22Linux+CI%22)
```
> sudo apt-get install curl unzip tar libxi-dev libxinerama-dev libxcursor-dev
> ./vcpkg_linux.sh
> ./build_linux.sh
```

Assuming you have installed the [Vulkan SDK](https://vulkan.lunarg.com/), the build should work out-of-the box for Visual Studio. On Linux, you may have to manually install additional dependencies for `vcpkg_linux.sh` to succeed.
