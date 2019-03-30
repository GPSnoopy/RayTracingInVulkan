mkdir build
cd build || goto :error
git clone https://github.com/Microsoft/vcpkg.git vcpkg.windows || goto :error
cd vcpkg.windows || goto :error
call bootstrap-vcpkg.bat || goto :error

vcpkg.exe install boost-exception:x64-windows-static || goto :error
vcpkg.exe install boost-program-options:x64-windows-static || goto :error
vcpkg.exe install boost-stacktrace:x64-windows-static || goto :error
vcpkg.exe install glfw3:x64-windows-static || goto :error
vcpkg.exe install glm:x64-windows-static || goto :error
vcpkg.exe install imgui:x64-windows-static || goto :error
vcpkg.exe install stb:x64-windows-static || goto :error
vcpkg.exe install tinyobjloader:x64-windows-static || goto :error

cd ..
cd ..

exit /b


:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
