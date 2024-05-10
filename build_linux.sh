#!/bin/sh
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

nproc=$(nproc || echo 4)

mkdir --parents build/linux
cd build/linux
cmake -D Boost_NO_SYSTEM_PATHS=ON -D CMAKE_BUILD_TYPE=Release -D VCPKG_TARGET_TRIPLET="${vcpkg_arch}-linux" -D CMAKE_TOOLCHAIN_FILE=../vcpkg.linux/scripts/buildsystems/vcpkg.cmake ../..
make -j${nproc}
