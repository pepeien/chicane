#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"
#include "Chicane/Core/FileSystem/Item.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        constexpr auto SEPARATOR = std::filesystem::path::preferred_separator;

        bool exists(const std::string& inPath);
        std::vector<Item> ls(const std::string& inDir = ".", std::uint32_t inDepth = 0);

        void openFolderDialog(const Dialog& inProps, Dialog::Callback inCallback);
        void openFileDialog(const FileDialog& inProps, Dialog::Callback inCallback);

        std::vector<char> readFile(const std::string& inFilepath);
        std::vector<unsigned char> readFileUnsigned(const std::string& inFilepath);
        ImageData* readImageFromFile(const std::string& inFilepath);
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