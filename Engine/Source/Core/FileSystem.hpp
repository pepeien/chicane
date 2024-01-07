#pragma once

#include "Base.hpp"

#include "FileSystem/Pak.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        namespace Paths
        {
            std::string rootDir();
            std::string contentDir();
        };

        std::vector<char> readFile(const std::string& inFilepath);
    }
}