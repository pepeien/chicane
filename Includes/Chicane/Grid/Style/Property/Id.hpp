#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StylePropertyId : std::uint8_t
        {
            Opacity,
            Width,
            Height,
            MinWidth,
            MinHeight,
            MaxWidth,
            MaxHeight,
            ZIndex,
            ForegroundColor,
            BackgroundColor,
            FontSize,
            LetterSpacing,
            Filter,
            BackdropFilter,
            Transform,
            Translate,
            Rotate,
            Scale,
            TransformOrigin,
            MarginTop,
            MarginBottom,
            MarginLeft,
            MarginRight,
            PaddingTop,
            PaddingBottom,
            PaddingLeft,
            PaddingRight,
            BorderTopWidth,
            BorderRightWidth,
            BorderBottomWidth,
            BorderLeftWidth,
            BorderTopColor,
            BorderRightColor,
            BorderBottomColor,
            BorderLeftColor,
            GapTop,
            GapBottom,
            GapLeft,
            GapRight,
            Count
        };
    }

    CHICANE_GRID String toString(Grid::StylePropertyId inValue);
}
