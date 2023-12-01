#pragma once

#include <string>

enum ELog
{
	INFO,
	WARNING,
	ERROR,
	CRITICAL
};

namespace Engine
{
	namespace Core
	{
		class Log
		{
		public:
			void info(std::string message);
			void warning(std::string message);
			void error(std::string message);
			void critical(std::string message);

		private:
			void emmit(ELog type, std::string message);
		};
	}
}