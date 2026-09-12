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
            All       = Fill | Wireframe | Bounds | Traces | Colliders | Skeletons
        };

        inline String toString(RendererFeature inValue)
        {
            switch (inValue)
            {
            case RendererFeature::None:
                return "None";

            case RendererFeature::Fill:
                return "Fill";

            case RendererFeature::Wireframe:
                return "Wireframe";

            case RendererFeature::Bounds:
                return "Bounds";

            case RendererFeature::Traces:
                return "Traces";

            case RendererFeature::Colliders:
                return "Colliders";

            case RendererFeature::Skeletons:
                return "Skeletons";

            case RendererFeature::All:
                return "All";

            default:
                return "";
            }
        }

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
}
