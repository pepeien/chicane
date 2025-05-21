#pragma once

#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            struct CHICANE_BOX Parsed
            {
            public:
                Mat4 matrix       = Mat4::One;
                Vec4 textureIndex = Vec4::Zero;
            };
        }
    }
}