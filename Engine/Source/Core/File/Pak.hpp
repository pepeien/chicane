#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace FileSystem
    {
        namespace Pak
        {
            enum class Type : uint8_t
            {
                Undefined,
                Mesh
            };

            enum class EntryType : uint8_t
            {
                Undefined,

                // Mesh
                Model,
                Texture
            };

            struct RootHeader
            {
            public:
                std::string signature = "PAK";
                std::string version   = "1.0";
                Type type             = Type::Undefined;
                std::string id        = "";
                std::string filePath  = "";
                uint32_t entryCount   = 0;

            public:
                std::string getWritable();
            };

            struct EntryHeader
            {
            public:
                EntryType type            = EntryType::Undefined;
                uint32_t offset           = 0;
                std::string filePath      = "";
                bool isCompressed         = false;

            public:
                std::string getWritable();
            };

            struct Entry
            {
                EntryType type;
                std::string filePath;
            };

            struct WriteInfo
            {
                Type type;
                std::string name;
                std::vector<Entry> entries;
                std::string outputPath;
            };

            void write(const WriteInfo& inWriteInfo);
        }
    }
}