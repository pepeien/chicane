#include "Pak.hpp"

#include <iostream>
#include <fstream>

#include "Core/FileSystem.hpp"

#include "Renderer/Image.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        namespace Pak
        {
            std::string RootHeader::getWritable()
            {
                std::stringstream result;
                result << signature;
                result << " ";
                result << version;
                result << " ";
                result << std::to_string((uint8_t) type);
                result << " ";
                result << id;
                result << " ";
                result << filePath;
                result << " ";
                result << entryCount;

                return result.str();
            }

            std::string EntryHeader::getWritable()
            {
                std::stringstream result;
                result << std::to_string((uint8_t) type);
                result << " ";
                result << offset;
                result << " ";
                result << filePath;

                return result.str();
            }

            void write(const WriteInfo& inWriteInfo)
            {
                RootHeader rootHeader;
                rootHeader.type       = inWriteInfo.type;
                rootHeader.id         = inWriteInfo.name;
                rootHeader.filePath   = inWriteInfo.outputPath + inWriteInfo.name + ".pak";
                rootHeader.entryCount = inWriteInfo.entries.size();

                std::ofstream file;

                file.open(rootHeader.filePath);

                file << rootHeader.getWritable();

                uint32_t offset = 0;

                for (Entry entry : inWriteInfo.entries)
                {
                    EntryHeader entryHeader;
                    entryHeader.type     = entry.type;
                    entryHeader.filePath = entry.filePath;
                    entryHeader.offset   = offset;

                    file << entryHeader.getWritable();

                    if (entry.type == EntryType::Model)
                    {
                        std::vector<char> sourceData = FileSystem::readFile(entry.filePath);

                        file << std::string(sourceData.begin(), sourceData.end());

                        offset += sizeof(char) * sourceData.size();
                    }

                    if (entry.type == EntryType::Texture)
                    {
                        int width = 0;
                        int height = 0;
                        int channels = 0;

                        std::string sourceData = std::string(
                            reinterpret_cast<char*>(
                                FileSystem::readImage(
                                    width,
                                    height,
                                    channels,
                                    entry.filePath
                                )
                            )
                        );

                        file << sourceData;

                        offset += sizeof(char) * sourceData.size();
                    }
                }

                file.close();
            }
        }
    }
}