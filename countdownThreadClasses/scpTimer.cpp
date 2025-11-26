#include "scpTimer.h"
#include <iostream>

namespace scp {

Timer::Timer(std::chrono::seconds duration, std::mutex& out_mx)
  : duration_(duration), out_mx_(out_mx) {}

Timer::~Timer() {
    stop();
    join();
}

void Timer::start() {
    deadline_ = Clock::now() + duration_;
    th_ = std::thread(&Timer::run_, this);
}

void Timer::stop() {
    stop_.store(true, std::memory_order_relaxed);
}

void Timer::join() {
    if (th_.joinable()) th_.join();
}

void Timer::run_() {
    while (!stop_.load(std::memory_order_relaxed)) {
        auto now = Clock::now();
        if (now >= deadline_) break;
        auto remaining =
            std::chrono::duration_cast<std::chrono::seconds>(deadline_ - now).count();
        {
            std::lock_guard<std::mutex> lk(out_mx_);
            std::cout << "[timer] " << remaining << " s left\n";
        }
        std::this_thread::sleep_until(now + std::chrono::seconds(1));
    }
    {
        std::lock_guard<std::mutex> lk(out_mx_);
        std::cout << "[timer] done\n";
    }
    done_.store(true, std::memory_order_relaxed);
}

} // namespace scp
