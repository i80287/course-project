#include "Logger.hpp"

namespace AppSpace::GraphicsUtils::DrawerUtils {

void Logger::DebugLog(std::string_view msg, std::source_location location) const
    noexcept(!kIsDebugLogEnabled) {
    if constexpr (DebugModeEnabled()) {
        std::clog << "[DEBUG] [message: '" << msg
                  << "'] [source: " << location.file_name() << ", line "
                  << location.line() << ':' << location.column() << "] ["
                  << location.function_name() << ']' << std::endl;
    }
}

void Logger::DebugLog(std::string_view str1, std::string_view str2, char sep,
                      std::source_location location) const
    noexcept(!kIsDebugLogEnabled) {
    if constexpr (DebugModeEnabled()) {
        bool add_separator = sep != '\0';
        std::string s(str1.size() + add_separator + str2.size(), '\0');
        s += str1;
        if (add_separator) {
            s.push_back(sep);
        }
        s += str2;
        DebugLog(s, location);
    }
}

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
