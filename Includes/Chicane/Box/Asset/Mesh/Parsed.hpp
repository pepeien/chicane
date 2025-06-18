#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX MeshParsed
        {
        public:
            Mat4          matrix       = Mat4::One;
            std::uint32_t textureIndex = 0;

            std::uint32_t _padding[3];
        };
    }
}