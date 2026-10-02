#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER BackendViewTargetSlot
        {
        public:
            String        name      = "";
            Draw::Id      texture   = Draw::InvalidId;
            RHI::Image    color     = {};
            RHI::Image    depth     = {};
            RHI::Buffer   camera    = {};
            RHI::Buffer   light     = {};
            RHI::Buffer   instances = {};
            RHI::Buffer   particles = {};
            std::uint32_t width     = 0;
            std::uint32_t height    = 0;
        };
    }
}
