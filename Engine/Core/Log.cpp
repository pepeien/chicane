#include "Log.h"

#include <ostream>
#include <iostream>

namespace Engine
{
    namespace Core
    {
		void Log::info(std::string message)
		{
			emmit(ELog::INFO, message);
		}

		void Log::warning(std::string message)
		{
			emmit(ELog::WARNING, message);
		}

		void Log::error(std::string message)
		{
			emmit(ELog::ERROR, message);
		}

		void Log::critical(std::string message)
		{
			emmit(ELog::CRITICAL, message);
		}

		void Log::emmit(ELog type, std::string message)
		{
			if (type == ELog::INFO)
			{
				std::cout << "[LOG] " << message << std::endl;

				return;
			}

			if (type == ELog::WARNING)
			{
				std::cout << "[WARNING] " << message << std::endl;

				return;
			}

			if (type == ELog::ERROR)
			{
				std::cerr << "[ERROR] " << message << std::endl;

				return;
			}

			std::cerr << "[CRITICAL] " << message << std::endl;
		}
	}
}