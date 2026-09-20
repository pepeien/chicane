#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLTextureTableEntry
        {
        public:
            std::uint32_t classIndex = 0;
            std::uint32_t layer      = 0;
            std::uint32_t width      = 1;
            std::uint32_t height     = 1;
        };
    }
}
