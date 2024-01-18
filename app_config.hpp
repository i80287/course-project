#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#if defined(likely)
#undef likely
#endif
#if defined(unlikely)
#undef unlikely
#endif

#if defined(__GNUC__)
#define likely(x)   __builtin_expect(bool(x), true)
#define unlikely(x) __builtin_expect(bool(x), false)
#else
#define likely(x)   bool(x)
#define unlikely(x) bool(x)
#endif

namespace AppSpace {

/// @brief ACTrie related constants
inline constexpr char kMinTrieChar = 'A';
inline constexpr char kMaxTrieChar = 'z';
inline constexpr char kIsCaseInsensetive = false;

}  // namespace AppSpace

#endif  // !APP_CONFIG_HPP
