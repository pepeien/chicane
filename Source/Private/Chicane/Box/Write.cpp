#include "Chicane/Box/Write.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        std::string WriteRootHeader::toString()
        {
            std::stringstream data;
            data << version;
            data << DATA_SEPARATOR;
            data << std::to_string(static_cast<uint8_t>(type));
            data << DATA_SEPARATOR;
            data << name;
            data << DATA_SEPARATOR;
            data << entryCount;
            data << DATA_SEPARATOR;

            std::stringstream result;
            result << HEADER_SIGNATURE;
            result << DATA_SEPARATOR;
            result << data.str(); // TODO Implement encryptor

            return result.str();
        }

        std::string WriteEntryHeader::toString()
        {
            std::stringstream data;
            data << std::to_string(static_cast<uint8_t>(type));
            data << DATA_SEPARATOR;
            data << std::to_string(vendor);
            data << DATA_SEPARATOR;

            std::stringstream result;
            result << ENTRY_SIGNATURE;
            result << data.str(); // TODO Implement encryptor

            return result.str();
        }

        void write(const WriteInfo& inWriteInfo)
        {
            WriteRootHeader rootHeader {};
            rootHeader.type       = inWriteInfo.type;
            rootHeader.name       = inWriteInfo.name;
            rootHeader.filePath   = inWriteInfo.outputPath + inWriteInfo.name + FILE_EXTENSTION;
            rootHeader.entryCount = static_cast<uint32_t>(inWriteInfo.entries.size());

            std::ofstream file(rootHeader.filePath, std::ios::out | std::ios::binary);

            file << rootHeader.toString();

            for (WriteEntry entry : inWriteInfo.entries)
            {
                WriteEntryHeader entryHeader;
                entryHeader.type   = entry.type;
                entryHeader.vendor = entry.vendor;

                file << entryHeader.toString();
                file << ENTRY_DATA_SIGNATURE;

                if (!entry.reference.empty())
                {
                    file << entry.reference;

                    continue;
                }

                std::vector<char> rawData = FileSystem::readFile(entry.dataFilePath);

                file << std::string(rawData.begin(), rawData.end());
            }

            file.close();
        }
    }
}