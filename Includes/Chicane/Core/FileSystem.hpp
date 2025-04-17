#pragma once

#include "Core/Essentials.hpp"
#include "Core/FileSystem/Dialog.hpp"
#include "Core/FileSystem/Dialog/FileDialog.hpp"
#include "Core/FileSystem/Item.hpp"
#include "Core/Image.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        static constexpr auto SEPARATOR = std::filesystem::path::preferred_separator;

        CHICANE bool exists(const std::string& inPath);
        CHICANE std::vector<Item> ls(const std::string& inDir = ".", std::uint32_t inDepth = 0);

        CHICANE void openFolderDialog(const Dialog& inProps, Dialog::Callback inCallback);
        CHICANE void openFileDialog(const FileDialog& inProps, Dialog::Callback inCallback);

        CHICANE std::vector<char> readFile(const std::string& inFilepath);
        CHICANE std::vector<unsigned char> readFileUnsigned(const std::string& inFilepath);
        CHICANE Image::Pixels readImageFromFile(const std::string& inFilepath);
        CHICANE Image::Pixels readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        );
        CHICANE Image::Pixels readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const Image::RawData& inData
        );
    }
}