#pragma once

#include "Base.hpp"

static const std::string HEADER_SIGNATURE     = "KERB";
static const std::string ENTRY_SIGNATURE      = "ENTRY";
static const std::string ENTRY_DATA_SIGNATURE = "DATA";
static const std::string DATA_SEPARATOR       = ";";
static const std::string FILE_EXTENSTION      = ".kerb";


namespace Engine
{
    namespace Kerb
    {
        enum class Type : uint8_t
        {
            Undefined,
            Mesh,
            CubeMap
        };

        enum class EntryType : uint8_t
        {
            Undefined,

            // Mesh
            Model,
            Texture
        };

        struct WriteRootHeader
        {
        public:
            std::string version   = "1.0";
            uint8_t type          = static_cast<uint8_t>(Type::Undefined);
            std::string name      = "";
            std::string filePath  = "";
            uint32_t entryCount   = 0;

        public:
            std::string toString();
        };

        struct WriteEntryHeader
        {
        public:
            uint8_t type    = static_cast<uint8_t>(EntryType::Undefined);
            uint8_t vendor  = 0;

        public:
            std::string toString();
        };

        struct WriteEntry
        {
        public:
            uint8_t type;
            uint8_t vendor;
            std::string filePath;
        };

        struct WriteInfo
        {
        public:
            uint8_t type;
            std::string name;
            std::vector<WriteEntry> entries;
            std::string outputPath;
        };

        struct Entry
        {
        public:
            uint8_t type;
            uint8_t vendor;
            std::vector<unsigned char> data;

        public:
            void parse(const std::string& inRawData);
        };

        struct Instance
        {
        public:
            uint8_t type;
            std::string name;
            uint32_t entryCount;
            std::vector<Entry> entries;
        };

        Instance read(const std::string& inFilePath);
        void write(const WriteInfo& inWriteInfo);
    }
}