#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        namespace Pak
        {
            struct Header
            {
                char signature[4]  = { "PAK" };
                char id[64]        = {};
                char path[256]     = {};
                float version      = 1.0f;
                uint32_t entryCount = 0;
            };

            struct Entry
            {
                char path[256]            = {};
                bool isCompressed         = false;
                uint32_t unCompressedSize = 0;
                uint32_t compressedSize   = 0;
                uint32_t offset           = 0;
            };

            void write(const std::vector<std::string>& inInputPaths, const std::string& inOutputPath);
        }
    }
}