#include "scpTimer.h"
#include "scpSum.h"
#include <chrono>
#include <iostream>
#include <mutex>

int main() {
#if defined(_WIN32)
    const char* os = "Windows";
#elif defined(__APPLE__)
    const char* os = "macOS";
#elif defined(__linux__)
    const char* os = "Linux";
#else
    const char* os = "Unknown OS";
#endif
    std::cout << "Running on " << os << '\n';

    std::mutex cout_mx;

    // 5-second timer
    scp::Timer timer(std::chrono::seconds(5), cout_mx);
    timer.start();

    // Do additions until the same deadline; print partial every 1s
    scp::Sum sum;
    auto result = sum.run_until(timer.deadline(), cout_mx, std::chrono::seconds(1));

    timer.join();

    std::lock_guard<std::mutex> lk(cout_mx);
    std::cout << "[main] finished: iterations=" << result.iterations
              << ", sum=" << result.sum << '\n';
    return 0;
}
