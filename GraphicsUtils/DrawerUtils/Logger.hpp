#pragma once

#include <iostream>
#include <source_location>

namespace AppSpace::GraphicsUtils::DrawerUtils {

class Logger final {
public:
    void DebugLog(std::string_view msg,
                  const std::source_location location =
                      std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled) {
        if constexpr (kIsDebugLogEnabled) {
            std::clog << "[DEBUG] [message: '" << msg
                      << "'] [source: " << location.file_name() << ", line "
                      << location.line() << ':' << location.column() << "] ["
                      << location.function_name() << ']' << std::endl;
        }
    }

    void DebugLog(std::string_view str1, std::string_view str2, char sep = '\0',
                  const std::source_location location =
                      std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled) {
        if constexpr (kIsDebugLogEnabled) {
            std::string s(str1.size() + str2.size(), '\0');
            s += str1;
            if (sep != '\0') {
                s.push_back(sep);
            }
            s += str2;
            DebugLog(s, location);
        }
    }

private:
    static constexpr bool kIsDebugLogEnabled =
#ifdef NDEBUG
        false;
#else
        true;
#endif
};

inline constexpr Logger logger;

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
