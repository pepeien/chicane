#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Color
    {
        // Types
        using Rgb                                        = Vec<3, std::uint8_t>;
        using Rgba                                       = Vec<4, std::uint8_t>;

        // Keyword
        static inline constexpr const char  HEX_KEYWORD  = '#';
        static inline constexpr const char* RGB_KEYWORD  = "rgb";
        static inline constexpr const char* RGBA_KEYWORD = "rgba";

        // Hex
        static inline constexpr const char* HEX_COLOR_TRANSPARENT = "#00000000";
        static inline constexpr const char* HEX_COLOR_RED         = "#F73B3BFF";
        static inline constexpr const char* HEX_COLOR_GREEN       = "#1FED4FFF";
        static inline constexpr const char* HEX_COLOR_BLUE        = "#4E8DDEFF";
        static inline constexpr const char* HEX_COLOR_LIME        = "#07E8B7FF";
        static inline constexpr const char* HEX_COLOR_YELLOW      = "#E8E805FF";
        static inline constexpr const char* HEX_COLOR_ORANGE      = "#FC850DFF";
        static inline constexpr const char* HEX_COLOR_CYAN        = "#05AEB0FF";
        static inline constexpr const char* HEX_COLOR_WHITE       = "#FFFFFFFF";
        static inline constexpr const char* HEX_COLOR_BLACK       = "#000000FF";

        // Text
        static inline constexpr const char* TEXT_COLOR_TRANSPARENT =
            "transparent";
        static inline constexpr const char* TEXT_COLOR_BLACK = "black";
        static inline constexpr const char* TEXT_COLOR_RED   = "red";
        static inline constexpr const char* TEXT_COLOR_GREEN = "green";
        static inline constexpr const char* TEXT_COLOR_BLUE  = "blue";
        static inline constexpr const char* TEXT_COLOR_WHITE = "white";

        CHICANE_CORE bool areEquals(const Rgba& inA, const Rgba& inB);

        CHICANE_CORE bool isVisible(const String& inValue);
        CHICANE_CORE bool isVisible(const Rgba& inValue);

        CHICANE_CORE Rgba toRgba(const String& inValue);
    }
}