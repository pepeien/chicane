#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class ComponentDirty : std::uint8_t
        {
            None     = 0,
            Style    = 1 << 0,
            Layout   = 1 << 1,
            Paint    = 1 << 2,
            Text     = 1 << 3,
            LaidOut  = 1 << 4,
            Insets   = 1 << 5,
            LiveBind = 1 << 6
        };

        inline constexpr ComponentDirty operator|(ComponentDirty inLeft, ComponentDirty inRight)
        {
            return static_cast<ComponentDirty>(static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight));
        }

        inline constexpr ComponentDirty operator&(ComponentDirty inLeft, ComponentDirty inRight)
        {
            return static_cast<ComponentDirty>(static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight));
        }

        inline ComponentDirty& operator|=(ComponentDirty& inLeft, ComponentDirty inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline ComponentDirty& operator&=(ComponentDirty& inLeft, ComponentDirty inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline constexpr ComponentDirty operator~(ComponentDirty inValue)
        {
            return static_cast<ComponentDirty>(~static_cast<std::uint8_t>(inValue));
        }

        inline constexpr bool has(ComponentDirty inFlags, ComponentDirty inFlag)
        {
            return (inFlags & inFlag) != ComponentDirty::None;
        }
    }

    CHICANE_GRID String toString(Grid::ComponentDirty inValue);
}
