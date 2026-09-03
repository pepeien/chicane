#include "Chicane/Core/FileSystem.hpp"

#include <fstream>

#include "Chicane/Core/FileSystem/Item/Type.reflected.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        bool exists(const Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return false;
            }

            return std::filesystem::exists(inPath);
        }

        std::vector<Item> ls(const Path& inDir, std::uint32_t inDepth)
        {
            if (inDir.isEmpty())
            {
                return {};
            }

            const std::uint32_t depth = inDepth == 0 ? 1 : inDepth;

            std::vector<Item> result;

            for (const auto& entry : std::filesystem::directory_iterator(inDir))
            {
                Item item(entry.is_directory() ? ItemType::Folder : ItemType::File, entry.path());
                if (item.type == ItemType::Folder && depth > 1)
                {
                    item.children = ls(item.path, depth - 1);
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
            std::ifstream file(inFilepath, std::ios::binary | std::ios::ate);

            if (!file)
            {
                throw std::runtime_error("Failed to open the file [" + inFilepath.toString() + "]");
            }

            std::streamsize size = file.tellg();
            file.seekg(0);

            std::vector<unsigned char> result(size);

            if (!file.read(reinterpret_cast<char*>(result.data()), size))
            {
                throw std::runtime_error("Failed to read the file [" + inFilepath.toString() + "]");
            }

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
            const Path path = std::filesystem::absolute(inFilepath);

            std::basic_ifstream<char> file(path.toString(), std::ios::ate | std::ios::binary);

            if (!file)
            {
                throw std::runtime_error("Failed to open the file [" + inFilepath.toString() + "]");
            }

            size_t            fileSize = (size_t)file.tellg();
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

            std::ofstream file(inFilepath.toString(), std::ios::binary);

            if (!file)
            {
                throw std::runtime_error("Failed to write the file [" + inFilepath.toString() + "]");
            }

            file.write((const char*)inData.data(), sizeof(unsigned char) * inData.size());
            file.flush();
            file.close();
        }

        void write(const std::vector<char>& inData, const Path& inFilepath)
        {
            if (inData.empty())
            {
                return;
            }

            std::ofstream file(inFilepath.toString(), std::ios::binary);

            if (!file)
            {
                throw std::runtime_error("Failed to write the file [" + inFilepath.toString() + "]");
            }

            file.write(inData.data(), sizeof(char) * inData.size());
            file.flush();
            file.close();
        }
    }
}