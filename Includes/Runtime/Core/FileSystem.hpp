#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct DirectoryResult
        {
            std::string path = "";
        };

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

        DirectoryResult openDirectoryDialog();

        FileResult openFileDialog(
            const std::string& inTitle,
            const std::vector<FileFormat>& inFileFormats
        );

        void ls(const std::string& inDir = ".");

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