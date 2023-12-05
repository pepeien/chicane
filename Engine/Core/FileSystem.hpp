#pragma once

#include "Base.hpp"

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