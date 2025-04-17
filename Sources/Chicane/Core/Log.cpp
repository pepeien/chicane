#include "Core/Log.hpp"

static constexpr const char* LOG_COLOR_START = "\33[";
static constexpr const char* LOG_COLOR_END   = "\33[0m\n";

constexpr std::uint32_t MAX_LOG_COUNT = 500;

namespace Chicane
{
    namespace Log
    {
        std::unordered_map<std::string, std::string> m_colors = {};

        std::deque<Entry>                        m_logs           = {};
        std::unique_ptr<Observable<const List*>> m_logsObservable = std::make_unique<Observable<const List*>>();

        Subscription<const List*>* watchLogs(
            std::function<void (const List*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        )
        {
            Subscription<const List*>* subscription = m_logsObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(&m_logs);

            return subscription;
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

            std::string hexColor = inHexColor;
            std::transform(
                hexColor.begin(),
                hexColor.end(),
                hexColor.begin(),
                ::toupper
            );

            if (m_colors.find(hexColor) == m_colors.end())
            {
                Vec<3, std::uint32_t> rgbColor = Color::hexToRgb(inHexColor);

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

            Entry instance = {};
            instance.text  = message;
            instance.color = hexColor;

            m_logs.emplace_back(instance);

            m_logsObservable->next(&m_logs);
        }

        void info(const std::string& inMessage)
        {
            emmit(
                Color::White,
                "INFO",
                inMessage
            );
        }

        void warning(const std::string& inMessage)
        {
            emmit(
                Color::Yellow,
                "WARNING",
                inMessage
            );
        }

        void error(const std::string& inMessage)
        {
            emmit(
                Color::Orange,
                "ERROR",
                inMessage
            );
        }

        void critical(const std::string& inMessage)
        {
            emmit(
                Color::Red,
                "CRITICAL",
                inMessage
            );
        }
    }
}