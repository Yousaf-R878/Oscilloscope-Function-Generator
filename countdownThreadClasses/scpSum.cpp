#include "scpSum.h"
#include <iostream>

namespace scp {

Sum::Result Sum::run_until(Clock::time_point deadline,
                           std::mutex& out_mx,
                           std::chrono::seconds interval) {
    std::uint64_t iterations = 0;
    unsigned sum = 0, i = 1;

    auto next_tick = Clock::now() + interval;

    while (Clock::now() < deadline) {
        sum += i;
        if (++i == 1000) i = 1;
        ++iterations;

        auto now = Clock::now();
        if (now >= next_tick) {
            {
                std::lock_guard<std::mutex> lk(out_mx);
                std::cout << "[sum] partial sum=" << sum
                          << " (iter=" << iterations << ")\n";
            }
            next_tick += interval;
            if (next_tick > deadline) next_tick = deadline;
        }
    }
    return {sum, iterations};
}

} // namespace scp
