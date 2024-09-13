#include "Chicane/Box/Entry.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Core/Helper.hpp"

namespace Chicane
{
    namespace Box
    {
        void Entry::parse(const std::string& inRawData, bool bInIsReference)
        {
            std::vector<std::string> splittedEntry = Helper::splitString(
                inRawData,
                ENTRY_DATA_SIGNATURE
            );
            std::vector<std::string> splittedEntryHeader = Helper::splitString(
                splittedEntry[0],
                DATA_SEPARATOR
            );
            std::string entryData = splittedEntry[1];

            type   = static_cast<EntryType>(std::stoi(splittedEntryHeader[0]));
            vendor = static_cast<uint8_t>(std::stoi(splittedEntryHeader[1]));

            if (bInIsReference)
            {
                reference = entryData;

                return;
            }

            data = std::vector<unsigned char>(entryData.begin(), entryData.end());
        }
    }
}