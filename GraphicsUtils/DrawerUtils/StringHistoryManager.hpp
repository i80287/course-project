#include <cassert>
#include <string>
#include <string_view>
#include <vector>

namespace AppSpace::GraphicsUtils::DrawerUtils {

class StringHistoryManager {
    using Index = std::size_t;

public:
    template <class StrType>
    void AddString(StrType&& str) {
        history_.emplace_back(std::forward<StrType>(str));
        current_history_position_ = kInitialPosition;
    }
    constexpr bool Empty() const noexcept {
        return history_.empty();
    }
    auto begin() const noexcept {
        return history_.begin();
    }
    auto begin() noexcept {
        return history_.begin();
    }
    auto end() const noexcept {
        return history_.end();
    }
    auto end() noexcept {
        return history_.end();
    }
    void Clear() noexcept {
        history_.clear();
    }
    std::string_view MoveToPreviousInputAndRead() noexcept;
    std::string_view MoveToNextInputAndRead() noexcept;

private:
    std::vector<std::string> history_;
    static constexpr Index kInitialPosition = static_cast<Index>(-1);
    Index current_history_position_         = kInitialPosition;
};

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
