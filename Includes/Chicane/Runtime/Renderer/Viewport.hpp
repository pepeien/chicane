#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct Viewport
        {
        public:
            Vec2 size     = Vec2::Zero;
            Vec2 position = Vec2::Zero;
        };
    }
}