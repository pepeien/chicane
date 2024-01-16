#include "FileSystem.hpp"

namespace Engine
{
    namespace FileSystem
    {
        namespace Paths
        {
            std::string rootDir()
            {
                std::string installLocation = std::getenv(ENGINE_ENV_VARIABALE.c_str());

                if (installLocation.empty())
                {
                    throw std::runtime_error("Engine install location is invalid");
                }

                return installLocation;
            }

            std::string contentDir()
            {
                return rootDir() + "/Content/";
            }
        }

        std::vector<char> readFile(const std::string& inFilepath)
        {
            std::ifstream file(inFilepath, std::ios::ate | std::ios::binary);
    
            if (file.is_open() == false)
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

        stbi_uc* readImage(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::string& inFilepath
        )
        {
            stbi_uc* result = stbi_load(
                inFilepath.c_str(),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (result == nullptr)
            {
                throw std::runtime_error(
                    "Failed to load the pixels for " + inFilepath
                );
            }

            return result;
        }
    }
}