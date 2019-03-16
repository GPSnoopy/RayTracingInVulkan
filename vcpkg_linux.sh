#!/bin/bash
set -e

mkdir -p build
cd build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
./bootstrap-vcpkg.sh

./vcpkg install boost-exception:x64-linux
./vcpkg install boost-stacktrace:x64-linux
./vcpkg install glfw3:x64-linux
./vcpkg install glm:x64-linux
./vcpkg install imgui:x64-linux
./vcpkg install stb:x64-linux
./vcpkg install tinyobjloader:x64-linux
