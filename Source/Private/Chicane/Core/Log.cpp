#include "Chicane/Core/Log.hpp"

#define LOG_COLOR_RED    "\033[1;31m"
#define LOG_COLOR_GREEN  "\033[1;32m"
#define LOG_COLOR_BLUE   "\033[1;34m"
#define LOG_COLOR_YELLOW "\033[1;33m"
#define LOG_COLOR_CYAN   "\033[1;36m"
#define LOG_COLOR_ORANGE "\033[38;2;255;165;0m"
#define LOG_COLOR_WHITE  "\033[1;37m"

namespace Chicane
{
    namespace Log
    {
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
            if (!IS_DEBUGGING)
            {
                return;
            }

            switch (inColor)
            {
            case Color::Red:
                std::cout << LOG_COLOR_RED;
    
                break;
            
            case Color::Blue:
                std::cout << LOG_COLOR_BLUE;
    
                break;

            case Color::Green:
                std::cout << LOG_COLOR_GREEN;
    
                break;

            case Color::Yellow:
                std::cout << LOG_COLOR_YELLOW;
    
                break;

            case Color::Orange:
                std::cout << LOG_COLOR_ORANGE;
    
                break;

            case Color::Cyan:
                std::cout << LOG_COLOR_CYAN;
    
                break;

            default:
                std::cout << LOG_COLOR_WHITE;

                break;
            }

            std::cout << inMessage << "\033[0m\n";
        }
    }
}