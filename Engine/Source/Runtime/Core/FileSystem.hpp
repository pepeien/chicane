#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace FileSystem
			{
				std::vector<char> readShader(const std::string& inFilepath);
				std::vector<char> readFile(const std::string& inFilepath);
			}
		}
	}
}