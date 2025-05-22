#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RUNTIME Viewport
        {
        public:
            Vec<2, std::uint32_t> size     = Vec<2, std::uint32_t>(0);
            Vec2         position = Vec2::Zero;
        };
    }
}