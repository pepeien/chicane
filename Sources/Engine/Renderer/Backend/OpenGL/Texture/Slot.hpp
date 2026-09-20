#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLTextureSlot
        {
        public:
            std::uint32_t classIndex     = ~0u;
            std::uint32_t layer          = 0;
            std::uint32_t sourceWidth    = 0;
            std::uint32_t sourceHeight   = 0;
            std::uint32_t residentMinMip = ~0u;
        };
    }
}
