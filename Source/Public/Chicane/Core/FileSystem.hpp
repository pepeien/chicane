#pragma once

#include "Chicane/Base.hpp"

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

        enum class ListType : std::uint8_t
        {
            Undefined,
            Folder,
            File
        };

        struct ListItem
        {
        public:
            ListType type    = ListType::Undefined;
            std::string name = "";
            std::string path = "";

            // File only
            std::string extension = "";

            // Folder only
            std::uint32_t childCount = 0;
        };

        DirectoryResult openDirectoryDialog();

        FileResult openFileDialog(
            const std::string& inTitle,
            const std::vector<FileFormat>& inFileFormats
        );

        bool exists(const std::string& inPath);
        std::vector<ListItem> ls(const std::string& inDir = ".", std::uint32_t inDepth = 0);

        std::vector<char> readFile(const std::string& inFilepath);
        ImageData* readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        );
        ImageData* readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::vector<unsigned char>& inData
        );
    }
}