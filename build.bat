@echo off
echo Building Oscilloscope-Function-Generator...
echo.

REM Always try to set up x64 environment first to ensure compatibility with ftd2xx.lib

REM Try to find and setup Visual Studio
set "VS_FOUND=0"

REM Check for Visual Studio 2026 (version 18) first
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
REM Check for Visual Studio 2022
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
REM Check for Visual Studio 2019
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
)

if "%VS_FOUND%"=="0" (
	echo ERROR: Visual Studio not found in standard locations!
	echo.
	echo Please do one of the following:
	echo   1. Make sure you opened "Developer PowerShell for VS 2022" from Start Menu
	echo   2. Or manually run: "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	echo   3. Or check if Visual Studio installation completed successfully
	echo.
	echo Searching for Visual Studio installations...
	dir "C:\Program Files\Microsoft Visual Studio" /s /b 2>nul | findstr /i "vcvarsall.bat"
	dir "C:\Program Files (x86)\Microsoft Visual Studio" /s /b 2>nul | findstr /i "vcvarsall.bat"
	pause
	exit /b 1
)

:compile

echo Compiling for x64 (amd64)...
REM Check if amd64 subdirectory exists for the library, otherwise use root ftd2xx.lib
if exist "amd64\ftd2xx.lib" (
	cl /EHsc /std:c++17 /I. main.cpp FTDController.cpp FTDWriter.cpp FTDReader.cpp FTDCommand.cpp FTDCommandParser.cpp FTDOscilloscope.cpp FTDOscilloscopeThreaded.cpp /link amd64\ftd2xx.lib setupapi.lib advapi32.lib user32.lib /OUT:main.exe
) else (
	cl /EHsc /std:c++17 /I. main.cpp FTDController.cpp FTDWriter.cpp FTDReader.cpp FTDCommand.cpp FTDCommandParser.cpp FTDOscilloscope.cpp FTDOscilloscopeThreaded.cpp /link ftd2xx.lib setupapi.lib advapi32.lib user32.lib /OUT:main.exe /MACHINE:X64
)

if %ERRORLEVEL% EQU 0 (
	echo.
	echo Build successful! Run with: .\main.exe
) else (
	echo.
	echo Build failed!
)

pause

