#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"
#include "Chicane/Core/FileSystem/Dialog/FileDialog.hpp"
#include "Chicane/Core/FileSystem/Item.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        static constexpr auto SEPARATOR = std::filesystem::path::preferred_separator;

        CHICANE_CORE bool exists(const std::string& inPath);
        CHICANE_CORE std::vector<Item> ls(const std::string& inDir = ".", std::uint32_t inDepth = 0);

        CHICANE_CORE void openFolderDialog(const Dialog& inProps, Dialog::Callback inCallback);
        CHICANE_CORE void openFileDialog(const FileDialog& inProps, Dialog::Callback inCallback);

        CHICANE_CORE std::vector<char> readFile(const std::filesystem::path& inFilepath);
        CHICANE_CORE std::vector<char> readFile(const std::string& inFilepath);
        CHICANE_CORE std::vector<unsigned char> readFileUnsigned(const std::string& inFilepath);
        CHICANE_CORE Image::Pixels readImageFromFile(const std::string& inFilepath);
        CHICANE_CORE Image::Pixels readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        );
        CHICANE_CORE Image::Pixels readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const Image::RawData& inData
        );
        CHICANE_CORE void freeImage(Image::Pixels inImage);
    }
}