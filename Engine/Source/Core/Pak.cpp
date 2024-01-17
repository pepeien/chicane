#include "Pak.hpp"

#include <iostream>
#include <fstream>

#include "FileSystem.hpp"

#include "Renderer/Image.hpp"

namespace Engine
{
    namespace Pak
    {
        std::string WriteRootHeader::toString()
        {
            std::string inDivisor = ";";

            std::stringstream data;
            data << version;
            data << inDivisor;
            data << std::to_string(static_cast<uint8_t>(type));
            data << inDivisor;
            data << name;
            data << inDivisor;
            data << entryCount;
            data << inDivisor;

            std::stringstream result;
            result << "CHAK";
            result << inDivisor;
            result << data.str(); // TODO Implement encryptor

            return result.str();
        }

        std::string WriteEntryHeader::toString()
        {
            std::string inDivisor = ";";

            std::stringstream data;
            data << std::to_string(static_cast<uint8_t>(type));
            data << inDivisor;
            data << std::to_string(vendor);
            data << inDivisor;

            std::stringstream result;
            result << "ENTRY";
            result << data.str(); // TODO Implement encryptor

            return result.str();
        }

        void Entry::parse(const std::string& inRawData)
        {
            std::vector<std::string> splittedEntry = Helper::splitString(
                inRawData,
                "DATA"
            );
            std::vector<std::string> splittedEntryHeader = Helper::splitString(
                splittedEntry[0],
                ";"
            );
            std::string entryData = splittedEntry[1];

            type   = static_cast<uint8_t>(std::stoi(splittedEntryHeader[0]));
            vendor = static_cast<uint8_t>(std::stoi(splittedEntryHeader[1]));
            data   = std::vector<unsigned char>(entryData.begin(), entryData.end());
        }

        Instance read(const std::string& inFilePath)
        {
            std::vector<char> data = FileSystem::readFile(inFilePath);
            std::vector<std::string> splittedData = Helper::splitString(
                std::string(data.begin(), data.end()),
                "ENTRY"
            );

            std::vector<std::string> splittedRootHeader = Helper::splitString(
                splittedData[0],
                ";"
            );

            Instance result;
            result.type       = static_cast<uint8_t>(std::stoi(splittedRootHeader[2]));
            result.name       = splittedRootHeader[3];
            result.entryCount = static_cast<uint32_t>(std::stoi(splittedRootHeader[4]));
            result.entries.reserve(result.entryCount);

            for (uint32_t i = 1; i < (result.entryCount + 1); i++)
            {
                Entry entry;
                entry.parse(splittedData[i]);

                result.entries.push_back(entry);
            }

            return result;
        }

        void write(const WriteInfo& inWriteInfo)
        {
            WriteRootHeader rootHeader;
            rootHeader.type       = inWriteInfo.type;
            rootHeader.name         = inWriteInfo.name;
            rootHeader.filePath   = inWriteInfo.outputPath + inWriteInfo.name + ".pak";
            rootHeader.entryCount = inWriteInfo.entries.size();

            std::ofstream file(rootHeader.filePath, std::ios::out | std::ios::binary);

            file << rootHeader.toString();

            for (WriteEntry entry : inWriteInfo.entries)
            {
                WriteEntryHeader entryHeader;
                entryHeader.type     = entry.type;
                entryHeader.vendor   = entry.vendor;

                std::vector<char> rawData = FileSystem::readFile(entry.filePath);

                file << entryHeader.toString();
                file << "DATA";
                file << std::string(rawData.begin(), rawData.end());
            }

            file.close();
        }
    }
}