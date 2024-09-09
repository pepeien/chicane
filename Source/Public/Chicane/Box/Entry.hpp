#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class EntryType : uint8_t
        {
            Undefined,
            Model,
            Texture
        };

        struct Entry
        {
        public:
            void parse(const std::string& inRawData, bool isReference = false);

        public:
            EntryType                  type   = EntryType::Undefined;

            uint8_t                    vendor = 0; // Only pertinent if type is `EntryType::Model`

            std::vector<unsigned char> data {}; // Only pertinent if type is `EntryType::Model` or `Entry::Texture`
            std::string                reference = ""; // Only pertinent if type is `EntryType::Mesh`
        };
    }
}