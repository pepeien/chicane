#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class ComponentStatus : std::uint8_t
        {
            None     = 0,
            Hovered  = 1 << 0,
            Focused  = 1 << 1,
            Dragging = 1 << 2,
            Culled   = 1 << 3
        };

        inline constexpr ComponentStatus operator|(ComponentStatus inLeft, ComponentStatus inRight)
        {
            return static_cast<ComponentStatus>(static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight));
        }

        inline constexpr ComponentStatus operator&(ComponentStatus inLeft, ComponentStatus inRight)
        {
            return static_cast<ComponentStatus>(static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight));
        }

        inline ComponentStatus& operator|=(ComponentStatus& inLeft, ComponentStatus inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline ComponentStatus& operator&=(ComponentStatus& inLeft, ComponentStatus inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline constexpr ComponentStatus operator~(ComponentStatus inValue)
        {
            return static_cast<ComponentStatus>(~static_cast<std::uint8_t>(inValue));
        }

        inline constexpr bool has(ComponentStatus inStatus, ComponentStatus inFlag)
        {
            return (inStatus & inFlag) != ComponentStatus::None;
        }
    }

    CHICANE_GRID String toString(Grid::ComponentStatus inValue);
}
