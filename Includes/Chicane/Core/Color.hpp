#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Color
    {
        // Types
        using Rgb  = Vec<3, std::uint8_t>;
        using Rgba = Vec<4, std::uint8_t>;

        // Keyword
        /*
         * Template 1: "#RRGGBB"
         * Template 2: "#RRGGBBAA"
         */
        static constexpr inline const char  HEX_KEYWORD = '#';
        /*
         * Template 1: "rgb(r,g,b)"
         */
        static constexpr inline const char* RGB_KEYWORD = "rgb";
        /*
         * Template 1: "rgba(r,g,b,a)"
         * Template 2: "rgba(rgb(r,g,b),a)"
         */
        static constexpr inline const char* RGBA_KEYWORD = "rgba";

        // Hex
        static constexpr inline const char* HEX_COLOR_TRANSPARENT = "#00000000";
        static constexpr inline const char* HEX_COLOR_RED         = "#F73B3BFF";
        static constexpr inline const char* HEX_COLOR_GREEN       = "#1FED4FFF";
        static constexpr inline const char* HEX_COLOR_BLUE        = "#4E8DDEFF";
        static constexpr inline const char* HEX_COLOR_LIME        = "#07E8B7FF";
        static constexpr inline const char* HEX_COLOR_YELLOW      = "#E8E805FF";
        static constexpr inline const char* HEX_COLOR_ORANGE      = "#FC850DFF";
        static constexpr inline const char* HEX_COLOR_CYAN        = "#05AEB0FF";
        static constexpr inline const char* HEX_COLOR_WHITE       = "#FFFFFFFF";
        static constexpr inline const char* HEX_COLOR_GRAY        = "#7B7B7BFF";
        static constexpr inline const char* HEX_COLOR_BLACK       = "#000000FF";

        // Text
        static constexpr inline const char* TEXT_COLOR_TRANSPARENT = "transparent";
        static constexpr inline const char* TEXT_COLOR_RED         = "red";
        static constexpr inline const char* TEXT_COLOR_GREEN       = "green";
        static constexpr inline const char* TEXT_COLOR_BLUE        = "blue";
        static constexpr inline const char* TEXT_COLOR_LIME        = "lime";
        static constexpr inline const char* TEXT_COLOR_YELLOW      = "yellow";
        static constexpr inline const char* TEXT_COLOR_ORANGE      = "orange";
        static constexpr inline const char* TEXT_COLOR_CYAN        = "cyan";
        static constexpr inline const char* TEXT_COLOR_WHITE       = "white";
        static constexpr inline const char* TEXT_COLOR_GRAY        = "gray";
        static constexpr inline const char* TEXT_COLOR_BLACK       = "black";

        CHICANE_CORE bool areEquals(const Rgba& inA, const Rgba& inB);

        CHICANE_CORE bool isVisible(const String& inValue);
        CHICANE_CORE bool isVisible(const Rgba& inValue);

        CHICANE_CORE String normalizeColor(const String& inValue);
        CHICANE_CORE Rgba toRgba(const String& inValue);
    }
}