#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class RendererFeature : std::uint8_t
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

            Default = Fill | Light,
            All     = Fill | Wireframe | Bounds | Traces | Colliders | Skeletons | HDR | Light
        };

        inline RendererFeature operator|(RendererFeature inLeft, RendererFeature inRight)
        {
            return static_cast<RendererFeature>(static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight));
        }

        inline RendererFeature operator&(RendererFeature inLeft, RendererFeature inRight)
        {
            return static_cast<RendererFeature>(static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight));
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
            return static_cast<RendererFeature>(~static_cast<std::uint8_t>(inValue));
        }
    }

    inline String toString(Renderer::RendererFeature inValue)
    {
        switch (inValue)
        {
        case Renderer::RendererFeature::None:
            return "None";

        case Renderer::RendererFeature::Fill:
            return "Fill";

        case Renderer::RendererFeature::Wireframe:
            return "Wireframe";

        case Renderer::RendererFeature::Bounds:
            return "Bounds";

        case Renderer::RendererFeature::Traces:
            return "Traces";

        case Renderer::RendererFeature::Colliders:
            return "Colliders";

        case Renderer::RendererFeature::Skeletons:
            return "Skeletons";

        case Renderer::RendererFeature::HDR:
            return "HDR";

        case Renderer::RendererFeature::Light:
            return "Light";

        case Renderer::RendererFeature::Default:
            return "Default";

        case Renderer::RendererFeature::All:
            return "All";

        default:
            return "";
        }
    }
}
