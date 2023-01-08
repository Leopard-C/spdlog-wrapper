
set HOME=%cd%
set VS=%HOME%\build\VisualStudio

md %VS%\x86 %VS%\x64

cd %VS%\x86
REM cmake -G "Visual Studio 15 2017" -A Win32 ../../..
cmake -G "Visual Studio 16 2019" -A Win32 ../../..
REM cmake -G "Visual Studio 17 2022" -A Win32 ../../..

cd %VS%\x64
REM cmake -G "Visual Studio 15 2017" -A x64 ../../..
cmake -G "Visual Studio 16 2019" -A x64 ../../..
REM cmake -G "Visual Studio 17 2022" -A x64 ../../..
