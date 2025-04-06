@echo off
setlocal

if not defined VULKAN_SDK (
    echo [ERROR] VULKAN_SDK is not defined.
    pause
    exit /b 1
)

set GLSLC=%VULKAN_SDK%\Bin\glslc.exe

set OUT_DIR=spv
if not exist %OUT_DIR% mkdir %OUT_DIR%

echo [INFO] .vert, .frag, .comp shaders are being compiled...

for %%f in (shaders\*.vert shaders\*.frag shaders\*.comp) do (
    echo Compiling %%~nxf ...
    "%GLSLC%" "%%f" -o "%OUT_DIR%\%%~nxf.spv" --target-env=vulkan1.2
)

echo [INFO] Shaders compiled!
pause
