#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class RendererFeature : std::uint16_t
        {
            None      = 0,
            Fill      = 1 << 0,
            Wireframe = 1 << 1,
            Bounds    = 1 << 2,
            Traces    = 1 << 3,
            Colliders = 1 << 4,
            Skeletons = 1 << 5,
            HDR       = 1 << 6,
            Light     = 1 << 7,
            Outline   = 1 << 8,
            Bloom     = 1 << 9,

            Default = Fill | Light | Outline,
            All     = Fill | Wireframe | Bounds | Traces | Colliders | Skeletons | HDR | Light | Outline | Bloom
        };

        inline RendererFeature operator|(RendererFeature inLeft, RendererFeature inRight)
        {
            return static_cast<RendererFeature>(
                static_cast<std::uint16_t>(inLeft) | static_cast<std::uint16_t>(inRight)
            );
        }

        inline RendererFeature operator&(RendererFeature inLeft, RendererFeature inRight)
        {
            return static_cast<RendererFeature>(
                static_cast<std::uint16_t>(inLeft) & static_cast<std::uint16_t>(inRight)
            );
        }

        inline RendererFeature& operator|=(RendererFeature& inLeft, RendererFeature inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline RendererFeature& operator&=(RendererFeature& inLeft, RendererFeature inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline RendererFeature operator~(RendererFeature inValue)
        {
            return static_cast<RendererFeature>(~static_cast<std::uint16_t>(inValue));
        }
    }

    CHICANE_RENDERER String toString(Renderer::RendererFeature inValue);
}
