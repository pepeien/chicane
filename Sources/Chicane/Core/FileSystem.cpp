#include "Core/FileSystem.hpp"

#include "Core.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        bool exists(const std::string& inPath)
        {
            if (inPath.empty())
            {
                return false;
            }

            return std::filesystem::exists(inPath);
        }

        std::vector<Item> ls(const std::string& inDir, std::uint32_t inDepth)
        {
            if (inDir.empty())
            {
                return {};
            }

            if (inDepth > 1)
            {
                return {};
            }

            std::vector<Item> result = {};

            for (const auto& entry : std::filesystem::directory_iterator(inDir))
            {
                const auto& path = entry.path();

                Item item = {};
                item.type      = entry.is_directory() ? Item::Type::Folder : Item::Type::File;
                item.name      = path.filename().string();
                item.extension = path.extension().string();
                item.path      = path.string();

                if (item.type == Item::Type::Folder)
                {
                    item.childCount = static_cast<std::uint32_t>(ls(item.path, 1).size());
                }

                result.push_back(item);
            }

            return result;
        }

        void openFolderDialog(const Dialog& inProps, Dialog::Callback inCallback)
        {
/*
            if (!Application::hasWindow())
            {
                throw std::runtime_error("Unable to open a folder dialog without a active window");
            }

            std::string location = String::trim(inProps.location);


            SDL_ShowOpenFolderDialog(
                inCallback,
                nullptr,
                Application::getWindow()->instance,
                location.empty() ? nullptr : location.c_str(),
                inProps.canSelectMany
            );
*/
        }

        void openFileDialog(const FileDialog& inProps, Dialog::Callback inCallback)
        {
/*
            if (!Application::hasWindow())
            {
                throw std::runtime_error("Unable to open a file dialog without a active window");
            }

            std::string location = String::trim(inProps.location);

            SDL_ShowOpenFileDialog(
                inCallback,
                nullptr,
                Application::getWindow()->instance,
                inProps.filters,
                inProps.filterCount,
                location.empty() ? nullptr : location.c_str(),
                inProps.canSelectMany
            );
*/
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

        std::vector<unsigned char> readFileUnsigned(const std::string& inFilepath)
        {
            std::basic_ifstream<unsigned char> file(inFilepath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open file -> " + inFilepath);
            }
    
            size_t fileSize = (size_t)file.tellg();
            std::vector<unsigned char> buffer(fileSize);
    
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
    
            return buffer;
        }

        
        Image::Pixels readImageFromFile(const std::string& inFilepath)
        {
            int width   = 0;
            int height  = 0;
            int channel = 0;
            int format  = 0;

            return readImageFromFile(
                width,
                height,
                channel,
                format,
                inFilepath
            );
        }

        Image::Pixels readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        )
        {
            outFormat = STBI_rgb_alpha;

            Image::Pixels result = stbi_load(
                inFilepath.c_str(),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (!result)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }

        Image::Pixels readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const Image::RawData& inData
        )
        {
            Image::Pixels result = stbi_load_from_memory(
                &inData[0],
                static_cast<int>(inData.size()),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (!result)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }
    }
}