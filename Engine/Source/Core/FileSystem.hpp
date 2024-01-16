#pragma once

#include "Base.hpp"

namespace Engine
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