#pragma once

#include <cstdint>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER TextureStreamerDecodeResult
        {
        public:
            Draw::Id        id    = Draw::InvalidId;
            std::uint32_t   mip   = 0;
            Image::Instance image = {};
        };
    }
}
