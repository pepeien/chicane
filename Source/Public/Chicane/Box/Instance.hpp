#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Entry.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class Type : uint8_t
        {
            Undefined,
            Mesh,    // Two entries [Model, Texture] both being refNames
            CubeMap, // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
            Texture, // Image
            Model    // 3D Model
        };

        struct Instance
        {
        public:
            Type type                  = Type::Undefined;
            std::string name           = "";
            std::string filepath       = "";
            uint32_t entryCount        = 0;
            std::vector<Entry> entries {};
        
        public:
            static const Instance& empty();
        };
    }
}