# scp-countdown-demo (v2)

Cross-platform C++17 demo. A `Timer` thread prints a per-second countdown while the main thread performs additions **and prints a partial sum every 1s** until the same deadline, then reports results.

## Files
- `scpTimer.h/.cpp` — `scp::Timer` class (threaded countdown)
- `scpSum.h/.cpp` — `scp::Sum` class (busy-work additions with 1s progress)
- `scpCountdown.cpp` — main program
- `Makefile` — simple GCC/Clang build

## Build

### macOS (Homebrew GCC)
```bash
brew install gcc
g++-14 -std=c++17 -O2 -pthread -o scpCountdown scpCountdown.cpp scpTimer.cpp scpSum.cpp
./scpCountdown
```
> Tip: `/usr/bin/g++` on macOS is clang; use Homebrew's `g++-13/14` (check with `which g++-14`).

### Linux
```bash
g++ -std=c++17 -O2 -pthread -o scpCountdown scpCountdown.cpp scpTimer.cpp scpSum.cpp
./scpCountdown
```

### Windows (MSYS2 / MinGW-w64)
```bash
# In the MSYS2 MinGW64 shell
pacman -S --needed mingw-w64-x86_64-gcc
g++ -std=c++17 -O2 -pthread -o scpCountdown.exe scpCountdown.cpp scpTimer.cpp scpSum.cpp
./scpCountdown.exe
```

Or use the Makefile (macOS/Linux):
```bash
make       # builds ./scpCountdown
./scpCountdown
```
