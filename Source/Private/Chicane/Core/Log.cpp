#include "Chicane/Core/Log.hpp"

#include <iostream>

#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Grid.hpp"

constexpr auto LOG_COLOR_START = "\33[";
constexpr auto LOG_COLOR_END   = "\33[0m\n";

constexpr std::uint32_t MAX_LOG_COUNT = 500;

namespace Chicane
{
    namespace Log
    {
        std::unordered_map<std::string, std::string> m_colors = {};

        List m_logs = {};
        std::unique_ptr<Observable<List>> m_logsObservable = std::make_unique<Observable<List>>();

        void watchLogs(
            std::function<void (const List&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        )
        {
            m_logsObservable->subscribe(
                inNextCallback,
                inErrorCallback,
                inCompleteCallback
            )->next(m_logs);
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

            message = Utils::trim(message);

            std::string hexColor = inHexColor;
            std::transform(
                hexColor.begin(),
                hexColor.end(),
                hexColor.begin(),
                ::toupper
            );

            if (m_colors.find(hexColor) == m_colors.end())
            {
                Vec<3, std::uint32_t> rgbColor = Grid::hexToRgb(inHexColor);

                m_colors.insert(
                    std::make_pair(
                        hexColor,
                        "38;2;" + std::to_string(rgbColor.x) + ";" + std::to_string(rgbColor.y) + ";" + std::to_string(rgbColor.z) + "m"
                    )
                );
            }

            std::string terminalColor = m_colors.at(hexColor);

            if (IS_DEBUGGING)
            {
                std::cout << LOG_COLOR_START << terminalColor << message << LOG_COLOR_END;
            }

            // History
            if (m_logs.size() > MAX_LOG_COUNT)
            {
                m_logs.pop_front();
            }

            Instance instance {};
            instance.text  = message;
            instance.color = hexColor;

            m_logs.push_back(instance);

            m_logsObservable->next(m_logs);
        }

        void info(const std::string& inMessage)
        {
            emmit(
                COLOR_WHITE,
                "INFO",
                inMessage
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                COLOR_YELLOW,
                "WARNING",
                inMessage
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                COLOR_ORANGE,
                "ERROR",
                inMessage
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                COLOR_RED,
                "CRITICAL",
                inMessage
            );
        }
    }
}