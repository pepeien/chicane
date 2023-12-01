#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		class FileSystem
		{
		public:
			std::vector<char> FileSystem::readShader(const std::string& filepath);

		private:
			std::vector<char> FileSystem::readFile(const std::string& filepath);
		};
	};
}