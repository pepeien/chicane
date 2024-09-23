#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        std::vector<DialogResult> DialogResult::fromRaw(const char* const* inData)
        {
            std::vector<DialogResult> result {};

            const char* const* currentData = inData;

            while (true)
            {
                if (!currentData || !*currentData)
                {
                    break;
                }

                DialogResult block {};
                block.path = *currentData;

                result.push_back(block);

                currentData++;
            }

            return result;
        }

        bool exists(const std::string& inPath)
        {
            if (inPath.empty())
            {
                return false;
            }

            return std::filesystem::exists(inPath);
        }

        std::vector<ListItem> ls(const std::string& inDir, std::uint32_t inDepth)
        {
            std::vector<ListItem> result {};

            if (inDepth > 1)
            {
                return result;
            }

            for (const auto& entry : std::filesystem::directory_iterator(inDir))
            {
                const auto& path = entry.path();

                ListItem item {};
                item.type      = entry.is_directory() ? ListType::Folder : ListType::File;
                item.name      = path.filename().string();
                item.extension = path.extension().string();
                item.path      = path.string();

                if (item.type == ListType::Folder)
                {
                    item.childCount = ls(item.path, 1).size();
                }

                result.push_back(item);
            }

            return result;
        }

        void openFolderDialog(const Dialog& inProps, DialogCallback inCallback)
        {
            Window* window = getWindow();

            if (!window)
            {
                throw std::runtime_error("Unable to open a folder dialog without a active window");
            }

            std::string location = Utils::trim(inProps.location);


            SDL_ShowOpenFolderDialog(
                inCallback,
                nullptr,
                window->instance,
                location.empty() ? nullptr : location.c_str(),
                inProps.canSelectMany
            );
        }

        void openFileDialog(const FileDialog& inProps, DialogCallback inCallback)
        {
            Window* window = getWindow();

            if (!window)
            {
                throw std::runtime_error("Unable to open a file dialog without a active window");
            }

            std::string location = Utils::trim(inProps.location);

            SDL_ShowOpenFileDialog(
                inCallback,
                nullptr,
                window->instance,
                inProps.filters,
                inProps.filterCount,
                location.empty() ? nullptr : location.c_str(),
                inProps.canSelectMany
            );
        }

        std::vector<char> readFile(const std::string& inFilepath)
        {
            std::ifstream file(inFilepath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open file -> " + inFilepath);
            }
    
            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);
    
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
    
            return buffer;
        }

        ImageData* readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        )
        {
            outFormat = STBI_rgb_alpha;

            ImageData* result = stbi_load(
                inFilepath.c_str(),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (result == nullptr)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }

        ImageData* readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::vector<unsigned char>& inData
        )
        {
            ImageData* result = stbi_load_from_memory(
                &inData[0],
                static_cast<int>(inData.size()),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (result == nullptr)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }
    }
}