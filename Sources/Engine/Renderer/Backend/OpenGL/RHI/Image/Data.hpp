#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Image/Format.hpp"
#include "Chicane/Renderer/RHI/Image/Kind.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHIImageData
        {
        public:
            std::uint32_t    texture = 0;
            std::uint32_t    fbo     = 0;
            RHI::ImageKind   kind    = RHI::ImageKind::Color2D;
            RHI::ImageFormat format  = RHI::ImageFormat::RGBA8;
            std::uint32_t    width   = 1;
            std::uint32_t    height  = 1;
            std::uint32_t    layers  = 1;
            std::uint32_t    mips    = 1;
            bool             bOwned  = true;
            bool             bView   = false;
            std::uint32_t    layer   = 0;
        };
    }
}
