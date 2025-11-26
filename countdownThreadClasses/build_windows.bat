@echo off
echo Building countdownThreadClasses for Windows...
echo.

REM Try to find and setup Visual Studio x64 environment
set "VS_FOUND=0"

if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
	set "VS_FOUND=1"
)

if "%VS_FOUND%"=="0" (
	echo ERROR: Visual Studio not found!
	echo Please make sure Visual Studio is installed.
	pause
	exit /b 1
)

echo Compiling...
cl /EHsc /std:c++17 /O2 scpCountdown.cpp scpTimer.cpp scpSum.cpp /Fe:scpCountdown.exe

if %ERRORLEVEL% EQU 0 (
	echo.
	echo Build successful! Run with: scpCountdown.exe
) else (
	echo.
	echo Build failed!
)

pause

