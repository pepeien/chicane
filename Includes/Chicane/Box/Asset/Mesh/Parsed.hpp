#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX MeshParsed
        {
        public:
            Mat4          modelMatrix  = Mat4::One;
            std::uint32_t textureIndex = 0;

            std::uint32_t _padding[3];
        };
    }
}