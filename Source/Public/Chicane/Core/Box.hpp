#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Helper.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto HEADER_SIGNATURE     = "BOX";
        constexpr auto ENTRY_SIGNATURE      = "ENTRY";
        constexpr auto ENTRY_DATA_SIGNATURE = "DATA";
        constexpr auto DATA_SEPARATOR       = ";";
        constexpr auto FILE_EXTENSTION      = ".box";

        enum class Type : uint8_t
        {
            Undefined,
            Mesh,    // Two entries [Model, Texture] both being refNames
            CubeMap, // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
            Texture, // Image
            Model    // 3D Model
        };

        enum class EntryType : uint8_t
        {
            Undefined,
            Model,
            Texture
        };

        struct WriteRootHeader
        {
        public:
            std::string version   = "1.1";
            Type type             = Type::Undefined;
            std::string name      = "";
            std::string filePath  = "";
            uint32_t entryCount   = 0;

        public:
            std::string toString();
        };

        struct WriteEntryHeader
        {
        public:
            EntryType type       = EntryType::Undefined;
            std::uint8_t vendor  = 0;

        public:
            std::string toString();
        };

        struct WriteEntry
        {
        public:
            EntryType type = EntryType::Undefined;
            uint8_t vendor = 0;

            std::string dataFilePath = "";
            std::string reference    = "";
        };

        struct WriteInfo
        {
        public:
            Type type                       = Type::Undefined;
            std::string name                = "";
            std::vector<WriteEntry> entries {};
            std::string outputPath          = "";
        };

        struct Entry
        {
        public:
            EntryType type = EntryType::Undefined;

            uint8_t vendor = 0; // Only pertinent if type is `EntryType::Mesh`

            std::vector<unsigned char> data {}; // Only pertinent if type is `EntryType::Model` or `Entry::Texture`
            std::string reference           = ""; // Only pertinent if type is `EntryType::Mesh`

        public:
            void parse(const std::string& inRawData, bool isReference = false);
        };

        struct Instance
        {
        public:
            Type type                  = Type::Undefined;
            std::string name           = "";
            std::string filepath       = "";
            uint32_t entryCount        = 0;
            std::vector<Entry> entries {};
        };

        void parseHeader(const std::string& inRawData, Instance& outInstance);
        Instance readHeader(const std::string& inFilePath);

        void parseData(const std::string& inRawData, Instance& outInstance);
        Instance read(const std::string& inFilePath);

        void write(const WriteInfo& inWriteInfo);
    }
}