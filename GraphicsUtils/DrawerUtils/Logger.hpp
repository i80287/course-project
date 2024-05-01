#pragma once

#include <iostream>
#include <source_location>
#include <string>

namespace AppSpace::GraphicsUtils::DrawerUtils {

class Logger final {
public:
    static constexpr bool DebugModeEnabled() noexcept;
    template <class T>
    void LogTypeName() const;
    void DebugLog(
        std::string_view msg,
        std::source_location location = std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled);
    void DebugLog(
        std::string_view str1, std::string_view str2, char sep = '\0',
        std::source_location location = std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled);
    template <class T>
    void DebugLog(
        std::string_view message, const T& arg,
        std::source_location location = std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled);

private:
    template <class T>
    static consteval std::string_view GetTypeName() {
        return GetTypeNameImpl(std::source_location::current());
    }

    static consteval std::string_view GetTypeNameImpl(
        std::source_location location) {
        const std::string_view s               = location.function_name();
        constexpr std::string_view type_id_str = "T = ";
        std::size_t lpos                       = s.find(type_id_str);
        if (lpos == std::string_view::npos) {
            return "";
        }
        lpos += type_id_str.size();
        std::size_t rpos = std::min(s.find(';', lpos), s.find(']', lpos));
        if (rpos == std::string_view::npos) {
            return "";
        }
        return s.substr(lpos, rpos - lpos);
    }

    static constexpr bool kIsDebugLogEnabled =
#ifdef NDEBUG
        false;
#else
        true;
#endif
};

constexpr bool Logger::DebugModeEnabled() noexcept {
    return kIsDebugLogEnabled;
}

template <class T>
void Logger::LogTypeName() const {
    std::clog << "[INFO] [type name: " << GetTypeName<T>() << ']' << std::endl;
}

template <class T>
void Logger::DebugLog(std::string_view message, const T& arg,
                      std::source_location location) const
    noexcept(!kIsDebugLogEnabled) {
    if constexpr (DebugModeEnabled()) {
        if constexpr (std::is_constructible_v<std::string_view, const T&>) {
            DebugLog(message, std::string_view(arg), '\0', location);
        } else {
            DebugLog(message, std::string_view(std::to_string(arg)), '\0',
                     location);
        }
    }
}

inline constexpr Logger logger;

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
