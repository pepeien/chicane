#include "Chicane/Core/Log.hpp"

#include "Chicane/Core/Event.hpp"

#define LOG_COLOR_RED    "\033[1;31m"
#define LOG_COLOR_GREEN  "\033[1;32m"
#define LOG_COLOR_BLUE   "\033[1;34m"
#define LOG_COLOR_YELLOW "\033[1;33m"
#define LOG_COLOR_CYAN   "\033[1;36m"
#define LOG_COLOR_ORANGE "\033[38;2;255;165;0m"
#define LOG_COLOR_WHITE  "\033[1;37m"

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
                Color::White,
                inMessage
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                Color::Yellow,
                inMessage
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                Color::Orange,
                inMessage
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                Color::Red,
                inMessage
            );
        }

        void emmit(
            Color inColor,
            const std::string& inMessage
        )
        {
            std::string terminalColor = LOG_COLOR_WHITE;
            std::string hexColor      = "#FFFFFF";

            switch (inColor)
            {
            case Color::Red:
                terminalColor = LOG_COLOR_RED;
                hexColor      = "#CC0000";
    
                break;
            
            case Color::Blue:
                terminalColor = LOG_COLOR_BLUE;
                hexColor      = "#3465A4";
    
                break;

            case Color::Green:
                terminalColor = LOG_COLOR_GREEN;
                hexColor      = "#1FED4F";
    
                break;

            case Color::Yellow:
                terminalColor = LOG_COLOR_YELLOW;
                hexColor      = "#C4A000";
    
                break;

            case Color::Orange:
                terminalColor = LOG_COLOR_ORANGE;
                hexColor      = "#FFA600";
    
                break;

            case Color::Cyan:
                terminalColor = LOG_COLOR_CYAN;
                hexColor      = "#06989A";
    
                break;
            }

            if (IS_DEBUGGING)
            {
                std::cout << terminalColor << inMessage + "\033[0m\n";
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
                        m_logs[i].text  = inMessage;
                        m_logs[i].color = hexColor;
                    }
                }
            }
            else
            {
                Instance instance {};
                instance.text  = inMessage;
                instance.color = hexColor;

                m_logs.push_back(instance);
            }

            m_logsObservable->next(m_logs);
        }
    }
}