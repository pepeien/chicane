#pragma once

#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
        public:
            Vec<3, float> position;
            Vec<3, float> color;
            Vec<2, float> texturePosition;
            Vec<3, float> normal;
        };
    }
}