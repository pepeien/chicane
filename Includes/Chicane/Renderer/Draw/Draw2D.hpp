#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Draw2D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw2D : Draw
        {
        public:
            using List = std::vector<Draw2D>;

        public:
            Draw2DInstance::List instances = {};
        };
    }
}