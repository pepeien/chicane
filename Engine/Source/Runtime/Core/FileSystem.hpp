#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace FileSystem
    {
        std::vector<char> readShader(const std::string& inFilepath);
        std::vector<char> readTexture(const std::string& inFilepath);
        std::string getRelativeTexturePath(const std::string& inFilepath);
        std::vector<char> readFile(const std::string& inFilepath);
        std::string getRelativePath(const std::string& inFilepath);
    }
}