#include "Log.hpp"

namespace Engine
{
    namespace Runtime
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
	
					std::time_t timestamp = std::time(0);
	
					if (type == MessageType::INFO)
					{
						std::cout << timestamp  << " [LOG] " << message << std::endl;
				
						return;
					}
				
					if (type == MessageType::WARNING)
					{
						std::cout << timestamp  << " [WARNING] " << message << std::endl;
				
						return;
					}
				
					if (type == MessageType::ERROR)
					{
						std::cerr << timestamp  << " [ERROR] " << message << std::endl;
				
						return;
					}
				
					std::cerr << timestamp  << " [CRITICAL] " << message << std::endl;
				}
			}		
		}
	}
}