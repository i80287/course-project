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
            std::clog << "[DEBUG] [Message: '" << msg
                      << "'] [source: " << location.file_name() << ", line "
                      << location.line() << ':' << location.column() << "] ["
                      << location.function_name() << ']' << std::endl;
        }
    }

private:
    static constexpr bool kIsDebugLogEnabled = true;
};

inline constexpr Logger logger;

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
