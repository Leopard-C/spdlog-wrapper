
@echo off

set HOME=%cd%
set VS=%HOME%\build\VisualStudio
set OUT_LIBDIR=%HOME%\..\spdlog-wrapper\lib\windows

set VS_VERSION="Visual Studio 16 2019"
REM 可选其他VS版本，例如：
REM set VS_VERSION="Visual Studio 15 2017"
REM set VS_VERSION="Visual Studio 17 2022"

if not exist "%VS%\Win32" (md "%VS%\Win32")
if not exist "%VS%\x64"   (md "%VS%\x64")

echo =================================================
echo ==                                             ==
echo ==                  Win32                      ==
echo ==                                             ==
echo =================================================
cd %VS%\Win32
cmake -G %VS_VERSION% -A Win32 ../../..
cmake --build . --config Debug
cmake --build . --config Release
if not exist "%OUT_LIBDIR%\Win32\Debug" (md "%OUT_LIBDIR%\Win32\Debug")
if not exist "%OUT_LIBDIR%\Win32\Release" (md "%OUT_LIBDIR%\Win32\Release")
copy Debug\spdlogd.lib "%OUT_LIBDIR%\Win32\Debug\spdlog.lib"
copy Debug\spdlogd.pdb "%OUT_LIBDIR%\Win32\Debug\spdlog.pdb"
copy Release\spdlog.lib "%OUT_LIBDIR%\Win32\Release\spdlog.lib"

echo =================================================
echo ==                                             ==
echo ==                   x64                       ==
echo ==                                             ==
echo =================================================
cd %VS%\x64
cmake -G %VS_VERSION% -A x64 ../../..
cmake --build . --config Debug
cmake --build . --config Release
if not exist "%OUT_LIBDIR%\x64\Debug" (md "%OUT_LIBDIR%\x64\Debug")
if not exist "%OUT_LIBDIR%\x64\Release" (md "%OUT_LIBDIR%\x64\Release")
copy Debug\spdlogd.lib "%OUT_LIBDIR%\x64\Debug\spdlog.lib"
copy Debug\spdlogd.pdb "%OUT_LIBDIR%\x64\Debug\spdlog.pdb"
copy Release\spdlog.lib "%OUT_LIBDIR%\x64\Release\spdlog.lib"
