#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CreateInfo
        {
        public:
            Vec2 resolution = Vec2::Zero;
            Vec2 position   = Vec2::Zero;
        };
    }
}