#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer/Draw/Poly/3D/Command.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER ViewTargetCommand
        {
        public:
            String            name    = "";
            std::uint32_t     width   = 0;
            std::uint32_t     height  = 0;
            DrawPoly3DCommand command = {};
        };
    }
}
