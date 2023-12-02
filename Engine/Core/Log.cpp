#include "Log.h"

namespace Engine
{
    namespace Core
    {
		namespace Log
		{
			void info(std::string message)
			{
				emmit(MessageType::INFO, message);
			}

			void warning(std::string message)
			{
				emmit(MessageType::WARNING, message);
			}

			void error(std::string message)
			{
				emmit(MessageType::ERROR, message);
			}

			void critical(std::string message)
			{
				emmit(MessageType::CRITICAL, message);
			}

			void emmit(MessageType type, std::string message)
			{
				if (IS_DEBUGGING == false)
				{
					return;
				}
			
				if (type == MessageType::INFO)
				{
					std::cout << "[LOG] " << message << std::endl;
			
					return;
				}
			
				if (type == MessageType::WARNING)
				{
					std::cout << "[WARNING] " << message << std::endl;
			
					return;
				}
			
				if (type == MessageType::ERROR)
				{
					std::cerr << "[ERROR] " << message << std::endl;
			
					return;
				}
			
				std::cerr << "[CRITICAL] " << message << std::endl;
			}
		}		
	}
}