#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Core/FileSystem/Item/Type.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        bool exists(const Path& inPath)
        {
            if (inPath.empty())
            {
                return false;
            }

            return std::filesystem::exists(inPath);
        }

        std::vector<Item> ls(const Path& inDir, std::uint32_t inDepth)
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
                item.type      = entry.is_directory() ? ItemType::Folder : ItemType::File;
                item.name      = path.filename().string();
                item.extension = path.extension().string();
                item.path      = path.string();

                if (item.type == ItemType::Folder)
                {
                    item.childCount = static_cast<std::uint32_t>(ls(item.path.toStandard(), 1).size());
                }

                result.push_back(item);
            }

            return result;
        }

        String readStringUnsigned(const Path& inFilepath)
        {
            const std::vector<unsigned char> raw = readUnsigned(inFilepath);

            if (raw.empty())
            {
                return "";
            }

            return String(raw.begin(), raw.end());
        }

        std::vector<unsigned char> readUnsigned(const Path& inFilepath)
        {
            std::basic_ifstream<unsigned char> file(
                inFilepath.string(),
                std::ios::ate | std::ios::binary
            );

            if (!file)
            {
                throw std::runtime_error(
                    String::sprint(
                        "Failed to open the file [%s]",
                        inFilepath.c_str()
                    ).toChar()
                );
            }
    
            std::vector<unsigned char> result = {};
            result.reserve(file.tellg());
    
            unsigned char character;

            file.seekg(0);

            while (file.read(&character, sizeof(unsigned char)))
            {
                result.push_back(character);
            }

            file.close();
    
            return result;
        }

        String readString(const Path& inFilepath)
        {
            const std::vector<char> raw = read(inFilepath);

            if (raw.empty())
            {
                return "";
            }

            return String(raw.begin(), raw.end());
        }

        std::vector<char> read(const Path& inFilepath)
        {
            std::basic_ifstream<char> file(
                inFilepath.string(),
                std::ios::ate | std::ios::binary
            );

            if (!file)
            {
                throw std::runtime_error(
                    String::sprint(
                        "Failed to open the file [%s]",
                        inFilepath.c_str()
                    ).toChar()
                );
            }
    
            size_t fileSize = (size_t)file.tellg();
            std::vector<char> result(fileSize);
            
            file.seekg(0);
            file.read(result.data(), fileSize);
            file.close();
    
            return result;
        }

        void write(const String& inData, const Path& inFilepath)
        {
            std::vector<unsigned char> data;
            std::copy(inData.begin(), inData.end(), std::back_inserter(data));

            write(data, inFilepath);
        }

        void write(const std::vector<unsigned char>& inData, const Path& inFilepath)
        {
            if (inData.empty())
            {
                return;
            }

            std::ofstream file(inFilepath.string(), std::ios::binary);

            if (!file)
            {
                throw std::runtime_error(
                    String::sprint(
                        "Failed to write the file [%s]",
                        inFilepath.c_str()
                    ).toChar()
                );
            }

            file.write((const char*) inData.data(), sizeof(unsigned char) * inData.size());
            file.flush();
            file.close();
        }

        void write(const std::vector<char>& inData, const Path& inFilepath)
        {
            if (inData.empty())
            {
                return;
            }

            std::ofstream file(inFilepath.string(), std::ios::binary);

            if (!file)
            {
                throw std::runtime_error(
                    String::sprint(
                        "Failed to write the file [%s]",
                        inFilepath.c_str()
                    ).toChar()
                );
            }

            file.write(inData.data(), sizeof(char) * inData.size());
            file.flush();
            file.close();
        }
    }
}