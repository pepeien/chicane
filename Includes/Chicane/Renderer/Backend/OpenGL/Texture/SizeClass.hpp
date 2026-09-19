#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLTextureSizeClass
        {
        public:
            std::uint32_t              size       = 1;
            std::uint32_t              texture    = 0;
            std::uint32_t              allocated  = 0;
            std::uint32_t              used       = 0;
            std::vector<std::uint32_t> freeLayers = {};
        };
    }
}
