#pragma once
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace scp {

class Timer {
public:
    using Clock = std::chrono::steady_clock;

    explicit Timer(std::chrono::seconds duration, std::mutex& out_mx);
    ~Timer();

    void start();   // spawn the timer thread
    void stop();    // signal early stop (optional)
    void join();    // wait for timer thread to finish

    Clock::time_point deadline() const { return deadline_; }
    bool done() const { return done_.load(std::memory_order_relaxed); }

private:
    void run_();

    std::chrono::seconds duration_;
    std::mutex& out_mx_;
    std::thread th_;
    std::atomic<bool> stop_{false};
    std::atomic<bool> done_{false};
    Clock::time_point deadline_{};
};

} // namespace scp
