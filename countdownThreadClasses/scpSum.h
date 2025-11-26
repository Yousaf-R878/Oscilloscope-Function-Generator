#pragma once
#include <chrono>
#include <cstdint>
#include <mutex>

namespace scp {

class Sum {
public:
    using Clock = std::chrono::steady_clock;
    struct Result { unsigned sum; std::uint64_t iterations; };

    // Busy-work additions until deadline; prints progress every `interval`
    Result run_until(Clock::time_point deadline,
                     std::mutex& out_mx,
                     std::chrono::seconds interval = std::chrono::seconds(1));
};

} // namespace scp
