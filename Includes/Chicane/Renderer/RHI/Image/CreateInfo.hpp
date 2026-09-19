#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Image/Format.hpp"
#include "Chicane/Renderer/RHI/Image/Kind.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER ImageCreateInfo
            {
            public:
                bool          bIsSampled = true;
                bool          bHasColor  = true;
                bool          bHasDepth  = false;

                ImageKind     kind   = ImageKind::Color2D;
                ImageFormat   format = ImageFormat::RGBA8;

                std::uint32_t width     = 1;
                std::uint32_t height    = 1;
                std::uint32_t layers    = 1;
                std::uint32_t mipLevels = 1;
            };
        }
    }
}
