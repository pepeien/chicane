#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Entry.hpp"
#include "Chicane/Box/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        struct WriteRootHeader
        {
        public:
            std::string toString();

        public:
            std::string version   = "1.1";
            Type        type      = Type::Undefined;
            std::string name      = "";
            std::string filePath  = "";
            uint32_t   entryCount = 0;
        };

        struct WriteEntryHeader
        {
        public:
            std::string toString();

        public:
            EntryType type       = EntryType::Undefined;
            std::uint8_t vendor  = 0;
        };

        struct WriteEntry
        {
            EntryType   type         = EntryType::Undefined;
            uint8_t     vendor       = 0;
            std::string dataFilePath = "";
            std::string reference    = "";
        };

        struct WriteInfo
        {
            Type                    type         = Type::Undefined;
            std::string             name         = "";
            std::vector<WriteEntry> entries      = {};
            std::string             outputFolder = "";
        };

        void write(const WriteInfo& inWriteInfo);
    }
}