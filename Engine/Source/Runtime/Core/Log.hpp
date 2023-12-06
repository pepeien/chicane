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

				void info(std::string message);
				void warning(std::string message);
				void error(std::string message);
				void critical(std::string message);
				void emmit(MessageType type, std::string message);
			}
		}
	}
}