#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Draw3D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw3D : Draw
        {
        public:
            using List = std::vector<Draw3D>;

        public:
            Draw3DInstance::List instances = {};
        };
    }
}