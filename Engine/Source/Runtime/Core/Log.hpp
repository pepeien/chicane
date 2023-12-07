#pragma once

#include "Base.hpp"

#include <ctime>

namespace Engine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Log
			{
				enum MessageType
				{
					INFO,
					WARNING,
					ERROR,
					CRITICAL
				};

				void info(std::string inMessage);
				void warning(std::string inMessage);
				void error(std::string inMessage);
				void critical(std::string inMessage);
				void emmit(MessageType inType, std::string inMessage);
			}
		}
	}
}