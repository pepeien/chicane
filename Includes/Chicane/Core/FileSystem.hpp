#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem/FolderDialog.hpp"
#include "Chicane/Core/FileSystem/FileDialog.hpp"
#include "Chicane/Core/FileSystem/Item.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        using Path = std::filesystem::path;

        static inline constexpr const auto SEPARATOR = Path::preferred_separator;

        CHICANE_CORE bool exists(const Path& inPath);
        CHICANE_CORE std::vector<Item> ls(const Path& inDir = ".", std::uint32_t inDepth = 0);

        CHICANE_CORE std::string readStringUnsigned(const Path& inFilepath);
        CHICANE_CORE std::vector<unsigned char> readUnsigned(const Path& inFilepath);
        CHICANE_CORE std::string readString(const Path& inFilepath);
        CHICANE_CORE std::vector<char> read(const Path& inFilepath);

        CHICANE_CORE void write(const std::string& inData, const Path& inFilepath);
        CHICANE_CORE void write(const std::vector<unsigned char>& inData, const Path& inFilepath);
        CHICANE_CORE void write(const std::vector<char>& inData, const Path& inFilepath);
    }
}