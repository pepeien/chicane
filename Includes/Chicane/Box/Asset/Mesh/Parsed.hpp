#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            struct CHICANE_BOX Parsed
            {
            public:
                Mat4          matrix       = Mat4::One;
                std::uint32_t textureIndex = 0;

                std::uint32_t _padding[3];
            };
        }
    }
}