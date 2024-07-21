#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Log
    {
        void info(const std::string& inMessage)
        {
            emmit(MessageType::INFO, inMessage);
        }

        void warning(const std::string& inMessage)
        {
            emmit(MessageType::WARNING, inMessage);
        }

        void error(const std::string& inMessage)
        {
            emmit(MessageType::ERROR, inMessage);
        }

        void critical(const std::string& inMessage)
        {
            emmit(MessageType::CRITICAL, inMessage);
        }

        void emmit(MessageType inType, const std::string& inMessage)
        {
            if (IS_DEBUGGING == false)
            {
                return;
            }

            std::time_t timestamp = std::time(0);

            if (inType == MessageType::INFO)
            {
                std::cout << timestamp  << " [LOG] " << inMessage << std::endl;
        
                return;
            }
        
            if (inType == MessageType::WARNING)
            {
                std::cout << timestamp  << " [WARNING] " << inMessage << std::endl;
        
                return;
            }
        
            if (inType == MessageType::ERROR)
            {
                std::cerr << timestamp  << " [ERROR] " << inMessage << std::endl;
        
                return;
            }
        
            std::cerr << timestamp  << " [CRITICAL] " << inMessage << std::endl;
        }
    }
}