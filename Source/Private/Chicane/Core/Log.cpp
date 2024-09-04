#include "Chicane/Core/Log.hpp"

#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Utils.hpp"

#define LOG_COLOR_START "\e["
#define LOG_COLOR_END   "\e[0m\n"

#define LOG_ANSI_COLOR_RED "38;2;212;25;25m"
#define LOG_HEX_COLOR_RED  "#F73B3B"

#define LOG_ANSI_COLOR_GREEN "38;2;31;237;79m"
#define LOG_HEX_COLOR_GREEN  "#1FED4F"

#define LOG_ANSI_COLOR_BLUE "38;2;78;141;222m"
#define LOG_HEX_COLOR_BLUE  "#4E8DDE"

#define LOG_ANSI_COLOR_YELLOW "38;2;232;232;5m"
#define LOG_HEX_COLOR_YELLOW  "#E8E805"

#define LOG_ANSI_COLOR_ORANGE "38;2;255;166;0m"
#define LOG_HEX_COLOR_ORANGE  "#FFA600"

#define LOG_ANSI_COLOR_CYAN "38;2;5;174;176m"
#define LOG_HEX_COLOR_CYAN  "#05AEB0"

#define LOG_ANSI_COLOR_WHITE "38;2;255;255;255m"
#define LOG_HEX_COLOR_WHITE  "#FFFFFF"

constexpr std::uint32_t MAX_LOG_COUNT = 500;

namespace Chicane
{
    namespace Log
    {
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
                Color::White
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                "WARNING",
                inMessage,
                Color::Yellow
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                "ERROR",
                inMessage,
                Color::Orange
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                "CRITICAL",
                inMessage,
                Color::Red
            );
        }

        void emmit(
            const std::string& inIdentifier,
            const std::string& inMessage,
            Color inColor
        )
        {
            std::string message = "[" + inIdentifier + "] " + inMessage;

            if (inIdentifier.empty())
            {
                message = inMessage;
            }

            message = Utils::trim(message);

            std::string terminalColor = LOG_ANSI_COLOR_WHITE;
            std::string hexColor      = LOG_HEX_COLOR_WHITE;

            switch (inColor)
            {
            case Color::Red:
                terminalColor = LOG_ANSI_COLOR_RED;
                hexColor      = LOG_HEX_COLOR_RED;

                break;

            case Color::Green:
                terminalColor = LOG_ANSI_COLOR_GREEN;
                hexColor      = LOG_HEX_COLOR_GREEN;

                break;

            case Color::Blue:
                terminalColor = LOG_ANSI_COLOR_BLUE;
                hexColor      = LOG_HEX_COLOR_BLUE;

                break;

            case Color::Yellow:
                terminalColor = LOG_ANSI_COLOR_YELLOW;
                hexColor      = LOG_HEX_COLOR_YELLOW;

                break;

            case Color::Orange:
                terminalColor = LOG_ANSI_COLOR_ORANGE;
                hexColor      = LOG_HEX_COLOR_ORANGE;

                break;

            case Color::Cyan:
                terminalColor = LOG_ANSI_COLOR_CYAN;
                hexColor      = LOG_HEX_COLOR_CYAN;

                break;
            }

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