#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPoly3DFlag : std::uint32_t
        {
            None       = 0,
            Lit        = 1 << 0,
            Shadow     = 1 << 1,
            Foreground = 1 << 2,
            Outlined   = 1 << 3
        };

        inline constexpr DrawPoly3DFlag operator|(DrawPoly3DFlag inLeft, DrawPoly3DFlag inRight)
        {
            return static_cast<DrawPoly3DFlag>(
                static_cast<std::uint32_t>(inLeft) | static_cast<std::uint32_t>(inRight)
            );
        }

        inline constexpr DrawPoly3DFlag operator&(DrawPoly3DFlag inLeft, DrawPoly3DFlag inRight)
        {
            return static_cast<DrawPoly3DFlag>(
                static_cast<std::uint32_t>(inLeft) & static_cast<std::uint32_t>(inRight)
            );
        }

        inline DrawPoly3DFlag& operator|=(DrawPoly3DFlag& inLeft, DrawPoly3DFlag inRight)
        {
            inLeft = inLeft | inRight;

            return inLeft;
        }

        inline DrawPoly3DFlag& operator&=(DrawPoly3DFlag& inLeft, DrawPoly3DFlag inRight)
        {
            inLeft = inLeft & inRight;

            return inLeft;
        }

        inline constexpr DrawPoly3DFlag operator~(DrawPoly3DFlag inValue)
        {
            return static_cast<DrawPoly3DFlag>(~static_cast<std::uint32_t>(inValue));
        }

        inline constexpr bool has(DrawPoly3DFlag inFlags, DrawPoly3DFlag inFlag)
        {
            return (inFlags & inFlag) != DrawPoly3DFlag::None;
        }
    }

    inline String toString(Renderer::DrawPoly3DFlag inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPoly3DFlag::None:
            return "None";

        case Renderer::DrawPoly3DFlag::Lit:
            return "Lit";

        case Renderer::DrawPoly3DFlag::Shadow:
            return "Shadow";

        case Renderer::DrawPoly3DFlag::Foreground:
            return "Foreground";

        case Renderer::DrawPoly3DFlag::Outlined:
            return "Outlined";

        default:
            return "";
        }
    }
}
