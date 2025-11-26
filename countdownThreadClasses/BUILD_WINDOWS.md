# Building countdownThreadClasses on Windows

## Method 1: Using the Build Script (Easiest)

1. Open **Developer PowerShell for VS 2022** (or VS 2026)
2. Navigate to the countdownThreadClasses directory:
   ```powershell
   cd "D:\Github Projects\Oscilloscope-Function-Generator\countdownThreadClasses"
   ```
3. Run the build script:
   ```powershell
   .\build_windows.bat
   ```
4. Run the executable:
   ```powershell
   .\scpCountdown.exe
   ```

## Method 2: Manual Compilation

1. Open **Developer PowerShell for VS 2022** (or VS 2026)
2. Navigate to the countdownThreadClasses directory
3. Set up the Visual Studio environment:
   ```powershell
   & "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
   ```
   (Adjust path if you have a different version)
4. Compile:
   ```powershell
   cl /EHsc /std:c++17 /O2 scpCountdown.cpp scpTimer.cpp scpSum.cpp /Fe:scpCountdown.exe
   ```
5. Run:
   ```powershell
   .\scpCountdown.exe
   ```

## Method 3: Using MinGW/MSYS2 (Alternative)

If you have MinGW-w64 installed:

```bash
g++ -std=c++17 -O2 -pthread -o scpCountdown.exe scpCountdown.cpp scpTimer.cpp scpSum.cpp
```

## What It Does

This program demonstrates multi-threading with:
- A **Timer** thread that counts down from 5 seconds
- The **main** thread that performs additions and prints progress every second
- Both threads run concurrently until the timer expires

## Expected Output

```
Running on Windows
[Timer] 5...
[Sum] partial sum=12345, iterations=1000000
[Timer] 4...
[Sum] partial sum=24680, iterations=2000000
[Timer] 3...
[Sum] partial sum=37015, iterations=3000000
[Timer] 2...
[Sum] partial sum=49350, iterations=4000000
[Timer] 1...
[Sum] partial sum=61685, iterations=5000000
[Timer] 0 - done
[main] finished: iterations=5000000, sum=61685
```

