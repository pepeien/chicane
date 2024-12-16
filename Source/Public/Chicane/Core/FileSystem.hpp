#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        typedef SDL_DialogFileCallback DialogCallback;

        struct DialogResult
        {
        public:
            static std::vector<DialogResult> fromRaw(const char* const* inData);

        public:
            std::filesystem::path path;
        };

        struct Dialog
        {
            std::string title         = "Dialog";
            std::string location      = "";

            bool        canSelectMany = false;
        };

        struct FileDialog : Dialog
        {
            const SDL_DialogFileFilter* filters     = nullptr;
            std::uint32_t               filterCount = 0;
        };

        enum class ListType : std::uint8_t
        {
            Undefined,
            Folder,
            File
        };

        struct ListItem
        {
            ListType      type      = ListType::Undefined;
            std::string   name      = "";
            std::string   path      = "";

            // File only
            std::string   extension = "";

            // Folder only
            std::uint32_t childCount = 0;
        };

        constexpr auto SEPARATOR = std::filesystem::path::preferred_separator;

        bool exists(const std::string& inPath);
        std::vector<ListItem> ls(const std::string& inDir = ".", std::uint32_t inDepth = 0);

        void openFolderDialog(const Dialog& inProps, DialogCallback inCallback);
        void openFileDialog(const FileDialog& inProps, DialogCallback inCallback);

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