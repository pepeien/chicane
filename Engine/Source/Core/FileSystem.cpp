#include "FileSystem.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        std::vector<char> readFile(const std::string& inFilepath)
        {
            std::string formattedFilepath = getRelativePath(inFilepath);

            std::ifstream file(formattedFilepath, std::ios::ate | std::ios::binary);
    
            if (file.is_open() == false)
            {
                throw std::runtime_error("Failed to open file -> " + formattedFilepath);
            }
    
            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);
    
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
    
            return buffer;
        }

        std::string getRelativePath(const std::string& inFilepath)
        {
            return ENGINE_DIR + inFilepath;
        }
    }
}