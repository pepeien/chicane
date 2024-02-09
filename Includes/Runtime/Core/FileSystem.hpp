#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        std::vector<char> readFile(const std::string& inFilepath);
        stbi_uc* readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::string& inFilepath
        );
        stbi_uc* readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::vector<unsigned char>& inData
        );
    }
}