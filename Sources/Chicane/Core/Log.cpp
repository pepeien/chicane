#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Log
    {
        static inline constexpr const char* LOG_COLOR_START = "\33[";
        static inline constexpr const char* LOG_COLOR_END   = "\33[0m\n";

        static inline constexpr const std::uint32_t MAX_LOG_COUNT = 500;

        static std::unordered_map<std::string, std::string> g_colors = {};

        static List           g_logs           = {};
        static ListObservable g_logsObservable = {};

        ListSubscription watchLogs(
            ListSubscription::NextCallback inNext,
            ListSubscription::ErrorCallback inError,
            ListSubscription::CompleteCallback inComplete
        )
        {
            return g_logsObservable
                .subscribe(inNext, inError, inComplete)
                .next(g_logs);
        }

        void emmit(
            const std::string& inHexColor,
            const std::string& inIdentifier,
            const std::string& inMessage
        )
        {
            std::string message = "[" + inIdentifier + "] " + inMessage;

            if (inIdentifier.empty())
            {
                message = inMessage;
            }

            message = String::trim(message);

            std::string color = inHexColor;
            std::transform(
                color.begin(),
                color.end(),
                color.begin(),
                ::toupper
            );

            if (g_colors.find(color) == g_colors.end())
            {
                Vec<3, std::uint32_t> rgbColor = Color::toRgba(inHexColor);

                g_colors.insert(
                    std::make_pair(
                        color,
                        "38;2;" +
                        std::to_string(rgbColor.r) + ";" +
                        std::to_string(rgbColor.g) + ";" +
                        std::to_string(rgbColor.b) + "m"
                    )
                );
            }

            const std::string& terminalColor = g_colors.at(color);

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

            g_logsObservable.next(g_logs);
        }

        void info(const std::string& inMessage)
        {
            emmit(
                Color::HEX_COLOR_WHITE,
                "INFO",
                inMessage
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                Color::HEX_COLOR_YELLOW,
                "WARNING",
                inMessage
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                Color::HEX_COLOR_ORANGE,
                "ERROR",
                inMessage
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                Color::HEX_COLOR_RED,
                "CRITICAL",
                inMessage
            );
        }
    }
}