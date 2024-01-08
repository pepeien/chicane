#pragma once

#include "Base.hpp"

#include "File/Pak.hpp"

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
        stbi_uc* readImage(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::string& inFilepath
        );
    }
}