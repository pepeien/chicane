#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace FileSystem
		{
			std::vector<char> readShader(const std::string& filepath);
			std::vector<char> readFile(const std::string& filepath);
		};
	};
}