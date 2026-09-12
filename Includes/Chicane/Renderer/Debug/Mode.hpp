#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DebugMode : std::uint8_t
        {
            None      = 0,
            Meshes    = 1 << 0,
            Bounds    = 1 << 1,
            Traces    = 1 << 2,
            Colliders = 1 << 3,
            Skeletons = 1 << 4,
            All       = Meshes | Bounds | Traces | Colliders | Skeletons
        };

        inline String toString(DebugMode inValue)
        {
            switch (inValue)
            {
            case DebugMode::None:
                return "None";

            case DebugMode::Meshes:
                return "Meshes";

            case DebugMode::Bounds:
                return "Bounds";

            case DebugMode::Traces:
                return "Traces";

            case DebugMode::Colliders:
                return "Colliders";

            case DebugMode::Skeletons:
                return "Skeletons";

            case DebugMode::All:
                return "All";

            default:
                return "";
            }
        }

        inline DebugMode operator|(DebugMode inLeft, DebugMode inRight)
        {
            return static_cast<DebugMode>(static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight));
        }

        inline DebugMode operator&(DebugMode inLeft, DebugMode inRight)
        {
            return static_cast<DebugMode>(static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight));
        }

        inline DebugMode& operator|=(DebugMode& inLeft, DebugMode inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline DebugMode& operator&=(DebugMode& inLeft, DebugMode inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline DebugMode operator~(DebugMode inValue)
        {
            return static_cast<DebugMode>(~static_cast<std::uint8_t>(inValue));
        }
    }
}
