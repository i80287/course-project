#pragma once

#include <chrono>

namespace AppSpace {

namespace chrono = std::chrono;

class Timer final {
    using TimePoint = chrono::time_point<chrono::high_resolution_clock>;
public:
    using Duration = std::chrono::milliseconds;

    Timer() noexcept : time_(chrono::high_resolution_clock::now()) {}
    Duration TimePassed() noexcept {
        return chrono::duration_cast<Duration>(chrono::high_resolution_clock::now() - time_);
    }
    Duration GetAndResetTime() noexcept {
        TimePoint time_point = time_;
        time_                = chrono::high_resolution_clock::now();
        return chrono::duration_cast<Duration>(time_ - time_point);
    }

private:
    TimePoint time_;
};

}  // namespace AppSpace
