mkdir build
cd build || goto :error
git clone https://github.com/Microsoft/vcpkg.git vcpkg.windows || goto :error
cd vcpkg.windows || goto :error
REM vcpkg 2022-02-16 12:35 UTC
git checkout 18e6e8855d2da137fada2387b6d137583c02b77c || goto :error
call bootstrap-vcpkg.bat || goto :error

vcpkg.exe install ^
	boost-exception:x64-windows-static ^
	boost-program-options:x64-windows-static ^
	boost-stacktrace:x64-windows-static ^
	freetype:x64-windows-static ^
	glfw3:x64-windows-static ^
	glm:x64-windows-static ^
	imgui:x64-windows-static ^
	stb:x64-windows-static ^
	tinyobjloader:x64-windows-static ^
	|| goto :error

cd ..
cd ..

exit /b


:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
