#pragma once

#include "Core.hpp"
#include "Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE Viewport
        {
        public:
            Vec<2, std::uint32_t> size     = Vec<2, std::uint32_t>(0);
            Vec<2, float>         position = Vec<2, float>(0.0f);
        };
    }
}