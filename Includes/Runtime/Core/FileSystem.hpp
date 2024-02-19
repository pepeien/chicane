#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct FileFormat
        {
            std::string title     = "";
            std::string extension = "";
        };

        struct FileResult
        {
            std::string path      = "";
            std::string extension = "";
        };

        FileResult openFileDialog(
            const std::string& inTitle,
            const std::vector<FileFormat>& inFileFormats
        );

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