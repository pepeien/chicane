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

        void info(const std::string& inMessage)
        {
            emmit(
                "INFO",
                inMessage,
                COLOR_WHITE
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                "WARNING",
                inMessage,
                COLOR_YELLOW
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                "ERROR",
                inMessage,
                COLOR_ORANGE
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                "CRITICAL",
                inMessage,
                COLOR_RED
            );
        }

        void emmit(
            const std::string& inIdentifier,
            const std::string& inMessage,
            const std::string& inHexColor
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
                std::uint32_t start = 0;
                std::uint32_t end   = m_logs.size() - 1;

                for (std::uint32_t i = end; i > 0; i--)
                {
                    if (i == start)
                    {
                        continue;
                    }

                    m_logs[i - 1] = m_logs[i];

                    if (i == end)
                    {
                        m_logs[i].text  = message;
                        m_logs[i].color = hexColor;
                    }
                }
            }
            else
            {
                Instance instance {};
                instance.text  = message;
                instance.color = hexColor;

                m_logs.push_back(instance);
            }

            m_logsObservable->next(m_logs);
        }
    }
}