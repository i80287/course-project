#pragma once

#include <iostream>
#include <source_location>

namespace AppSpace::GraphicsUtils::DrawerUtils {

class Logger final {
public:
    template <class T>
    void LogTypeName() const {
        std::clog << "[INFO] [type name: " << GetTypename<T>() << ']'
                  << std::endl;
    }

    void DebugLog(std::string_view msg,
                  const std::source_location location =
                      std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled) {
        if constexpr (kIsDebugLogEnabled) {
            std::clog << "[DEBUG] [message: '" << msg
                      << "'] [source: " << location.file_name()
                      << ", line " << location.line() << ':'
                      << location.column() << "] ["
                      << location.function_name() << ']' << std::endl;
        }
    }

    void DebugLog(std::string_view str1, std::string_view str2,
                  char sep = '\0',
                  const std::source_location location =
                      std::source_location::current()) const
        noexcept(!kIsDebugLogEnabled) {
        if constexpr (kIsDebugLogEnabled) {
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

private:
    template <class T>
    static consteval std::string_view GetTypename() {
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

inline constexpr Logger logger;

}  // namespace AppSpace::GraphicsUtils::DrawerUtils
