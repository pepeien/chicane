#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawParticle
        {
        public:
            using List = std::vector<DrawParticle>;

        public:
            Vec4 positionRotation = Vec4(0.0f);                   // xyz position, w rotation
            Vec4 sizeAge          = Vec4(1.0f, 1.0f, 0.0f, 1.0f); // xy size, z age, w additive
            Vec4 color            = Vec4(1.0f);
            Vec4 axis             = Vec4(0.0f); // xyz beam axis
        };
    }
}
