#include <cassert>
#include <string>
#include <string_view>
#include <vector>

namespace AppSpace::GraphicsUtils::DrawerUtils {

class StringHistoryManager {
    using Index       = std::size_t;
    using SignedIndex = std::ptrdiff_t;

public:
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

    template <class StrType>
    void AddString(StrType&& str) {
        history_.emplace_back(std::forward<StrType>(str));
        current_history_position_ = kInitialPosition;
    }
    constexpr bool Empty() const noexcept;
    std::string_view Last() const noexcept;
    void PopLast() noexcept;
    std::string PopAndGetLast() noexcept;
    void Clear() noexcept;
    std::string_view MoveToPreviousInputAndRead() noexcept;
    std::string_view MoveToNextInputAndRead() noexcept;

private:
    static constexpr Index kInitialPosition = static_cast<Index>(-1);
    static constexpr Index IndexToSignedIndex(SignedIndex i) noexcept {
        return static_cast<Index>(i);
    }

    std::vector<std::string> history_;
    Index current_history_position_ = kInitialPosition;
};

constexpr bool StringHistoryManager::Empty() const noexcept {
    return history_.empty();
}

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
