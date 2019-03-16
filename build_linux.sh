#!/bin/sh
set -e

mkdir --parents build/linux
cd build/linux
cmake -D VCPKG_TARGET_TRIPLET=x64-linux -D CMAKE_TOOLCHAIN_FILE=../vcpkg.linux/scripts/buildsystems/vcpkg.cmake ../..
make -j
