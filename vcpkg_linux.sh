#!/bin/bash
set -e

mkdir -p build
cd build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
git checkout 2020.11-1
./bootstrap-vcpkg.sh

./vcpkg install \
	boost-exception:x64-linux \
	boost-program-options:x64-linux \
	boost-stacktrace:x64-linux \
	glfw3:x64-linux \
	glm:x64-linux \
	imgui:x64-linux \
	stb:x64-linux \
	tinyobjloader:x64-linux
