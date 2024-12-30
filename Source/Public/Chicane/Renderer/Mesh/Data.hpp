#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Mesh
    {
        struct Data
        {
        public:
            Mat<4, float> transform;
            Vec<4, float> textureIndex;
        };
    }
}