#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX MeshParsed
        {
        public:
            Mat4          modelMatrix  = Mat4::One;
            Mat4          boundsMatrix = Mat4::One;
            std::uint32_t textureIndex = 0;

            std::uint32_t _padding[3];
        };
    }
}