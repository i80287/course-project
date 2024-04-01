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
    std::string_view Last() const noexcept;
    void PopLast() noexcept;
    constexpr bool Empty() const noexcept;
    std::string MoveAndPopLast() noexcept {
        assert(!Empty());
        std::string last_elem(std::move(history_.back()));
        history_.pop_back();
        return last_elem;
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

constexpr bool StringHistoryManager::Empty() const noexcept {
    return history_.empty();
}

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
