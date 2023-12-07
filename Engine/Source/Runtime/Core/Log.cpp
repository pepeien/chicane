#include "Log.hpp"

namespace Engine
{
    namespace Runtime
	{
		namespace Core
    	{
			namespace Log
			{
				void info(std::string inMessage)
				{
					emmit(MessageType::INFO, inMessage);
				}
	
				void warning(std::string inMessage)
				{
					emmit(MessageType::WARNING, inMessage);
				}
	
				void error(std::string inMessage)
				{
					emmit(MessageType::ERROR, inMessage);
				}
	
				void critical(std::string inMessage)
				{
					emmit(MessageType::CRITICAL, inMessage);
				}
	
				void emmit(MessageType inType, std::string inMessage)
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
	}
}