#include "Chicane/Core/Log.hpp"

#include <iostream>

namespace Chicane
{
    namespace Log
    {
        static inline constexpr const char* LOG_COLOR_START = "\33[";
        static inline constexpr const char* LOG_COLOR_END   = "\33[0m\n";

        static inline constexpr const std::uint32_t MAX_LOG_COUNT = 500;

        static inline std::unordered_map<String, String> g_colors = {};

        static inline List g_logs = {};

        const List& getLogs()
        {
            return g_logs;
        }

        void emmit(const String& inHexColor, const String& inIdentifier, const String& inMessage)
        {
            const String message = (inIdentifier.isEmpty() ? inMessage : "[" + inIdentifier + "] " + inMessage).trim();
            const String color   = inHexColor.toUpper();

            if (g_colors.find(color) == g_colors.end())
            {
                Color::Rgba rgbColor = Color::toRgba(inHexColor);

                g_colors.insert(
                    std::make_pair(
                        color,
                        "38;2;" + std::to_string(rgbColor.r) + ";" + std::to_string(rgbColor.g) + ";" +
                            std::to_string(rgbColor.b) + "m"
                    )
                );
            }

            const String& terminalColor = g_colors.at(color);

            if (IS_DEBUGGING)
            {
                std::cout << LOG_COLOR_START << terminalColor << message << LOG_COLOR_END;
            }

            // History
            if (g_logs.size() > MAX_LOG_COUNT)
            {
                g_logs.pop_front();
            }

            g_logs.emplace_back(message, color);
        }

        void info(const String& inMessage)
        {
            emmit(Color::HEX_COLOR_WHITE, "INFO", inMessage);
        }

        void warning(const String& inMessage)
        {
            emmit(Color::HEX_COLOR_YELLOW, "WARNING", inMessage);
        }

        void error(const String& inMessage)
        {
            emmit(Color::HEX_COLOR_ORANGE, "ERROR", inMessage);
        }

        void critical(const String& inMessage)
        {
            emmit(Color::HEX_COLOR_RED, "CRITICAL", inMessage);
        }
    }
}