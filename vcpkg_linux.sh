#!/bin/bash
set -e

arch=$(uname -m)
vcpkg_arch=""
case ${arch} in
	"aarch64")
		vcpkg_arch="arm64"
		if [ -z ${VCPKG_FORCE_SYSTEM_BINARIES} ]; then
			echo "VCPKG_FORCE_SYSTEM_BINARIES must be set to 1 on aarch64!"
			exit 1
		fi
		;;
	"x86_64")
		vcpkg_arch="x64"
		;;
	*)
		echo "Please check the mapping for 'uname -m' output to vcpkg expected arch"
		exit 1
		;;
esac

mkdir -p build
cd build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
git checkout 2025.06.13
./bootstrap-vcpkg.sh

./vcpkg install \
	boost-exception:${vcpkg_arch}-linux \
	boost-program-options:${vcpkg_arch}-linux \
	boost-stacktrace:${vcpkg_arch}-linux \
	glfw3:${vcpkg_arch}-linux \
	glm:${vcpkg_arch}-linux \
	imgui[core,freetype,glfw-binding,vulkan-binding]:${vcpkg_arch}-linux \
	stb:${vcpkg_arch}-linux \
	tinyobjloader:${vcpkg_arch}-linux
