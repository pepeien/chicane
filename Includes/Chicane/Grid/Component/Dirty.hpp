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

    inline String toString(Grid::ComponentDirty inValue)
    {
        switch (inValue)
        {
        case Grid::ComponentDirty::None:
            return "None";

        case Grid::ComponentDirty::Style:
            return "Style";

        case Grid::ComponentDirty::Layout:
            return "Layout";

        case Grid::ComponentDirty::Paint:
            return "Paint";

        case Grid::ComponentDirty::Text:
            return "Text";

        case Grid::ComponentDirty::LaidOut:
            return "LaidOut";

        case Grid::ComponentDirty::Insets:
            return "Insets";

        case Grid::ComponentDirty::LiveBind:
            return "LiveBind";

        default:
            return "";
        }
    }
}
