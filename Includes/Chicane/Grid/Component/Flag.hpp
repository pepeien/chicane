#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class ComponentFlag : std::uint8_t
        {
            None     = 0,
            Style    = 1 << 0,
            Layout   = 1 << 1,
            LaidOut  = 1 << 2,
            Insets   = 1 << 3,
            LiveBind = 1 << 4
        };

        inline String toString(ComponentFlag inValue)
        {
            switch (inValue)
            {
            case ComponentFlag::None:
                return "None";

            case ComponentFlag::Style:
                return "Style";

            case ComponentFlag::Layout:
                return "Layout";

            case ComponentFlag::LaidOut:
                return "LaidOut";

            case ComponentFlag::Insets:
                return "Insets";

            case ComponentFlag::LiveBind:
                return "LiveBind";

            default:
                return "";
            }
        }

        inline constexpr ComponentFlag operator|(ComponentFlag inLeft, ComponentFlag inRight)
        {
            return static_cast<ComponentFlag>(static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight));
        }

        inline constexpr ComponentFlag operator&(ComponentFlag inLeft, ComponentFlag inRight)
        {
            return static_cast<ComponentFlag>(static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight));
        }

        inline ComponentFlag& operator|=(ComponentFlag& inLeft, ComponentFlag inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline ComponentFlag& operator&=(ComponentFlag& inLeft, ComponentFlag inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline constexpr ComponentFlag operator~(ComponentFlag inValue)
        {
            return static_cast<ComponentFlag>(~static_cast<std::uint8_t>(inValue));
        }

        inline constexpr bool has(ComponentFlag inFlags, ComponentFlag inFlag)
        {
            return (inFlags & inFlag) != ComponentFlag::None;
        }
    }
}
