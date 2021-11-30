#!/bin/bash
set -e

mkdir -p build
cd build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
# vcpkg 2021-11-30 23:20 UTC
git checkout ed74ff32c6a859b6b9a96f1ee028dcfcae184a26
./bootstrap-vcpkg.sh

./vcpkg install \
	boost-exception:x64-linux \
	boost-program-options:x64-linux \
	boost-stacktrace:x64-linux \
	freetype:x64-linux \
	glfw3:x64-linux \
	glm:x64-linux \
	imgui:x64-linux \
	stb:x64-linux \
	tinyobjloader:x64-linux
