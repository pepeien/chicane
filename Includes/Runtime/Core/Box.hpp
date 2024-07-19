#pragma once

#include "Runtime/Runtime.hpp"

#include "Runtime/Core/FileSystem.hpp"
#include "Runtime/Core/Helper.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::string HEADER_SIGNATURE     = "BOX";
        static const std::string ENTRY_SIGNATURE      = "ENTRY";
        static const std::string ENTRY_DATA_SIGNATURE = "DATA";
        static const std::string DATA_SEPARATOR       = ";";
        static const std::string FILE_EXTENSTION      = ".box";

        enum class Type : uint8_t
        {
            Undefined,

            // Actor
            Mesh,

            // Skybox
            CubeMap
        };

        enum class EntryType : uint8_t
        {
            Undefined,

            // Actor
            Mesh,    // Usually is two entries [Model, Texture] both being the refName
            Texture,
            Model
        };

        struct WriteRootHeader
        {
        public:
            std::string version   = "1.0";
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
            EntryType type       = EntryType::Undefined;
            uint8_t vendor       = 0;
            std::string filePath = "";
        };

        struct WriteInfo
        {
        public:
            Type type                       = Type::Undefined;
            std::string name                = "";
            std::vector<WriteEntry> entries = {};
            std::string outputPath          = "";
        };

        struct Entry
        {
        public:
            EntryType type = EntryType::Undefined;

            uint8_t vendor = 0; // Only pertinent if type is `EntryType::Mesh`

            std::vector<unsigned char> data = {}; // Only pertinent if type is `EntryType::Mesh` or `Entry::Texture`
            std::string referenceName       = ""; // Only pertinent if type is `EntryType::Model`

        public:
            void parse(const std::string& inRawData);
        };

        struct Instance
        {
        public:
            Type type                  = Type::Undefined;
            std::string name           = "";
            uint32_t entryCount        = 0;
            std::vector<Entry> entries = {};
        };

        Instance read(const std::string& inFilePath);
        void write(const WriteInfo& inWriteInfo);
    }
}