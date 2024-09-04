#include "Chicane/Box/Parse.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Helper.hpp"

namespace Chicane
{
    namespace Box
    {
        void parseHeader(const std::string& inRawData, Instance& outInstance)
        {
            std::vector<std::string> splittedData = Helper::splitString(
                inRawData,
                ENTRY_SIGNATURE
            );

            std::vector<std::string> splittedRootHeader = Helper::splitString(
                splittedData[0],
                DATA_SEPARATOR
            );

            outInstance.type       = static_cast<Type>(std::stoi(splittedRootHeader[2]));
            outInstance.name       = splittedRootHeader[3];
            outInstance.entryCount = static_cast<uint32_t>(std::stoi(splittedRootHeader[4]));
        }

        Instance readHeader(const std::string& inFilePath)
        {
            std::vector<char> rawData = FileSystem::readFile(inFilePath);
            std::string data          = std::string(rawData.begin(), rawData.end());

            Instance result {};
            result.filepath = inFilePath;

            parseHeader(data, result);

            return result;
        }

        void parseData(const std::string& inRawData, Instance& outInstance)
        {
            std::vector<std::string> splittedData = Helper::splitString(
                inRawData,
                ENTRY_SIGNATURE
            );

            for (uint32_t i = 1; i < (outInstance.entryCount + 1); i++)
            {
                Entry entry;
                entry.parse(
                    splittedData[i],
                    outInstance.type == Type::Mesh || outInstance.type == Type::CubeMap
                );

                outInstance.entries.push_back(entry);
            }
        }

        Instance read(const std::string& inFilePath)
        {
            std::vector<char> rawData = FileSystem::readFile(inFilePath);
            std::string data          = std::string(rawData.begin(), rawData.end());

            Instance result {};
            result.filepath = inFilePath;

            parseHeader(data, result);
            parseData(data, result);

            return result;
        }
    }
}