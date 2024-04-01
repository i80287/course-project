#pragma once

#include <iostream>
#include <source_location>

namespace StaticLogger {

inline constexpr bool kIsDebugLogEnabled = true;

static void DebugLog(
    std::string_view msg,
    const std::source_location location =
        std::source_location::current()) noexcept(!kIsDebugLogEnabled) {
    if constexpr (kIsDebugLogEnabled) {
        std::clog << "[DEBUG] [Message: '" << msg << "'] [from:\n"
                  << "file " << location.file_name() << ", line "
                  << location.line() << ':' << location.column() << ' '
                  << location.function_name() << "]\n";
    }
}

}  // namespace StaticLogger
