#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        std::vector<char> readFile(const std::string& inFilepath);
        std::string getRelativePath(const std::string& inFilepath);
    }
}