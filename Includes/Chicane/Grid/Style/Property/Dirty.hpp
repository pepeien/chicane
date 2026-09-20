#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StylePropertyDirty : std::uint8_t
        {
            None    = 0,
            Layout  = 1 << 0,
            Paint   = 1 << 1,
            Text    = 1 << 2,
            Subtree = 1 << 3
        };

        inline constexpr StylePropertyDirty operator|(StylePropertyDirty inLeft, StylePropertyDirty inRight)
        {
            return static_cast<StylePropertyDirty>(
                static_cast<std::uint8_t>(inLeft) | static_cast<std::uint8_t>(inRight)
            );
        }

        inline constexpr StylePropertyDirty operator&(StylePropertyDirty inLeft, StylePropertyDirty inRight)
        {
            return static_cast<StylePropertyDirty>(
                static_cast<std::uint8_t>(inLeft) & static_cast<std::uint8_t>(inRight)
            );
        }

        inline StylePropertyDirty& operator|=(StylePropertyDirty& inLeft, StylePropertyDirty inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline StylePropertyDirty& operator&=(StylePropertyDirty& inLeft, StylePropertyDirty inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline constexpr StylePropertyDirty operator~(StylePropertyDirty inValue)
        {
            return static_cast<StylePropertyDirty>(~static_cast<std::uint8_t>(inValue));
        }

        inline constexpr bool has(StylePropertyDirty inDirty, StylePropertyDirty inFlag)
        {
            return (inDirty & inFlag) != StylePropertyDirty::None;
        }
    }

    inline String toString(Grid::StylePropertyDirty inValue)
    {
        switch (inValue)
        {
        case Grid::StylePropertyDirty::None:
            return "None";

        case Grid::StylePropertyDirty::Layout:
            return "Layout";

        case Grid::StylePropertyDirty::Paint:
            return "Paint";

        case Grid::StylePropertyDirty::Text:
            return "Text";

        case Grid::StylePropertyDirty::Subtree:
            return "Subtree";

        default:
            return "";
        }
    }
}
