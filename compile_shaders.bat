@echo off
setlocal

if not defined VULKAN_SDK (
    echo [ERROR] VULKAN_SDK 환경변수가 설정되어 있지 않습니다.
    pause
    exit /b 1
)

set GLSLC=%VULKAN_SDK%\Bin\glslc.exe

set OUT_DIR=spv
if not exist %OUT_DIR% mkdir %OUT_DIR%

echo [INFO] .vert, .frag, .comp 셰이더 컴파일 시작...

for %%f in (shaders\*.vert shaders\*.frag shaders\*.comp) do (
    echo Compiling %%~nxf ...
    "%GLSLC%" "%%f" -o "%OUT_DIR%\%%~nxf.spv" --target-env=vulkan1.3
)

echo [INFO] 셰이더 컴파일 완료!
pause
