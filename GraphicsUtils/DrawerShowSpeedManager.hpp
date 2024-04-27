#pragma once

#include <cassert>
#include <chrono>

namespace AppSpace::GraphicsUtils {

namespace chrono = std::chrono;

class DrawerShowSpeedManager final {
public:
    // clang-format off
        // We use 'int' instead of 'std::uint32_t' or 'std::int32_t'
        //  to match one to one with the following ImGui function:
        // bool ImGui::SliderInt(const char*,int*,int,int,const char*,ImGuiSliderFlags)
    // clang-format on
    using TimeUnit                          = int;
    static constexpr TimeUnit kMinSpeedUnit = 0;
    static constexpr TimeUnit kMaxSpeedUnit = 9;

    bool ShouldHandleNextEvent() {
        assert(kMinSpeedUnit <= current_show_speed_);
        assert(current_show_speed_ <= kMaxSpeedUnit);

        if (current_show_speed_ == kMinSpeedUnit) {
            return false;
        }

        const auto delay = kMaxTimeDelay *
                           (kMaxSpeedUnit - current_show_speed_) /
                           kDelayUnitScale;
        const auto time_now = ClockType::now();
        const auto time_since_last_event =
            time_now - time_since_last_event_handled_;
        if (time_since_last_event <= delay) {
            return false;
        }
        time_since_last_event_handled_ = time_now;
        return true;
    }

    constexpr TimeUnit& GetShowSpeedRef() noexcept {
        return current_show_speed_;
    }

private:
    using Duration                       = std::chrono::milliseconds;
    using ClockType                      = chrono::steady_clock;
    using TimePoint                      = ClockType::time_point;
    static constexpr auto kMaxTimeDelay  = Duration(1000);
    static constexpr auto kInfiniteDelay = Duration::max();

    static constexpr auto kDelayUnitScale = kMaxSpeedUnit - kMinSpeedUnit - 1;
    static_assert(kDelayUnitScale >= 0, "delay must be non-negative");
    static_assert((kDelayUnitScale & (kDelayUnitScale - 1)) == 0,
                  "must be power of 2 for better performance");

    TimeUnit current_show_speed_ = kMaxSpeedUnit;
    TimePoint time_since_last_event_handled_{};
};

}  // namespace AppSpace::GraphicsUtils
