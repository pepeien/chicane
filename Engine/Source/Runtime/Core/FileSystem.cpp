#include "FileSystem.hpp"

namespace Engine
{
	namespace Core
	{
		namespace FileSystem
		{
			std::vector<char> readShader(const std::string& filepath)
    		{
    		    return readFile(ENGINE_SHADERS_DIR + filepath);
    		}

			std::vector<char> readFile(const std::string& filepath)
    		{
    			std::string formattedFilepath = ENGINE_DIR + filepath;

    			std::ifstream file(formattedFilepath, std::ios::ate | std::ios::binary);

    			if (!file.is_open())
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
		}
	}
}