#pragma once

#include "Core/Math.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            struct CHICANE CompiledData
            {
            public:
                Mat<4, float> matrix       = Mat<4, float>(1.0f);
                Vec<4, float> textureIndex = Vec<4, float>(0.0f);
            };
        }
    }
}