#include "Pak.hpp"

#include <iostream>
#include <fstream>

#include "FileSystem.hpp"

#include "Renderer/Image.hpp"

namespace Engine
{
    namespace Pak
    {
        std::string WriteRootHeader::getWritable(const std::string& inDivisor)
        {
            std::stringstream structData;
            structData << inDivisor;
            structData << signature;
            structData << inDivisor;
            structData << version;
            structData << inDivisor;
            structData << std::to_string(static_cast<uint8_t>(type));
            structData << inDivisor;
            structData << id;
            structData << inDivisor;
            structData << filePath;
            structData << inDivisor;
            structData << entryCount;
            structData << inDivisor;

            std::string structDataString = structData.str();
            size_t structDataOffset      = structDataString.size();

            // Calculate the text offset ahead of time
            std::stringstream offsetData;
            offsetData << structDataOffset + std::to_string(structDataOffset).size();

            std::string offsetDataString = offsetData.str();

            std::stringstream result;
            result << structDataString;
            result << offsetDataString;

            return result.str();
        }

        std::string WriteEntryHeader::getWritable(const std::string& inDivisor)
        {
            std::stringstream structData;
            structData << inDivisor;
            structData << std::to_string(static_cast<uint8_t>(type));
            structData << inDivisor;
            structData << std::to_string(vendor);
            structData << inDivisor;
            structData << offset;
            structData << inDivisor;

            std::string structDataString = structData.str();
            size_t structDataOffset      = structDataString.size();

            // Calculate the text offset ahead of time
            std::stringstream offsetData;
            offsetData << inDivisor.size() + structDataOffset + std::to_string(structDataOffset).size();
            offsetData << inDivisor;

            std::string offsetDataString = offsetData.str();

            std::stringstream result;
            result << structDataString;
            result << offsetDataString;

            return result.str();
        }

        Instance read(const std::string& inFilePath)
        {
            std::vector<char> data = FileSystem::readFile(inFilePath);

            std::vector<std::string> splittedData = Helper::splitString(
                std::string(data.begin(), data.end()),
                ";"
            );

            Instance result;
            result.type = static_cast<uint8_t>(std::stoi(splittedData[2]));
            result.name = splittedData[4];
            result.entries.reserve(std::stoi(splittedData[6]));

            uint32_t offset = std::stoi(splittedData[7]);
            for (uint32_t i = offset; i < data.size(); i += offset)
            {
                splittedData = Helper::splitString(
                    std::string(std::next(data.begin(), i), data.end()),
                    ";"
                );

                uint8_t type          = static_cast<uint8_t>(std::stoi(splittedData[1]));
                uint8_t vendor        = static_cast<uint8_t>(std::stoi(splittedData[2]));
                uint32_t dataOffset   = std::stoi(splittedData[3]);
                uint32_t headerOffset = std::stoi(splittedData[4]);
                std::string rawData   = splittedData[5];

                Entry entry;
                entry.type   = type;
                entry.vendor = vendor;
                entry.data   = std::vector<char>(rawData.begin(), rawData.end());

                result.entries.push_back(entry);

                offset += dataOffset + headerOffset;
            }

            return result;
        }

        void write(const WriteInfo& inWriteInfo)
        {
            WriteRootHeader rootHeader;
            rootHeader.type       = inWriteInfo.type;
            rootHeader.id         = inWriteInfo.name;
            rootHeader.filePath   = inWriteInfo.outputPath + inWriteInfo.name + ".pak";
            rootHeader.entryCount = inWriteInfo.entries.size();

            std::ofstream file;

            file.open(rootHeader.filePath);

            file << rootHeader.getWritable();

            for (WriteEntry entry : inWriteInfo.entries)
            {
                WriteEntryHeader entryHeader;
                entryHeader.type     = entry.type;
                entryHeader.vendor   = entry.vendor;

                if (entry.type == static_cast<uint8_t>(EntryType::Model))
                {
                    std::vector<char> sourceData = FileSystem::readFile(entry.filePath);

                    entryHeader.offset = sizeof(char) * sourceData.size();

                    file << entryHeader.getWritable();
                    file << std::string(sourceData.begin(), sourceData.end());

                    continue;
                }

                if (entry.type == static_cast<uint8_t>(EntryType::Texture))
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

                    entryHeader.offset = sizeof(char) * sourceData.size();

                    file << entryHeader.getWritable();
                    file << sourceData;
    
                    continue;
                }
            }

            file.close();
        }
    }
}