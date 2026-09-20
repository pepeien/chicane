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

    inline String toString(Grid::StylePropertyId inValue)
    {
        switch (inValue)
        {
        case Grid::StylePropertyId::Opacity:
            return "Opacity";

        case Grid::StylePropertyId::Width:
            return "Width";

        case Grid::StylePropertyId::Height:
            return "Height";

        case Grid::StylePropertyId::MinWidth:
            return "MinWidth";

        case Grid::StylePropertyId::MinHeight:
            return "MinHeight";

        case Grid::StylePropertyId::MaxWidth:
            return "MaxWidth";

        case Grid::StylePropertyId::MaxHeight:
            return "MaxHeight";

        case Grid::StylePropertyId::ZIndex:
            return "ZIndex";

        case Grid::StylePropertyId::ForegroundColor:
            return "ForegroundColor";

        case Grid::StylePropertyId::BackgroundColor:
            return "BackgroundColor";

        case Grid::StylePropertyId::FontSize:
            return "FontSize";

        case Grid::StylePropertyId::LetterSpacing:
            return "LetterSpacing";

        case Grid::StylePropertyId::Filter:
            return "Filter";

        case Grid::StylePropertyId::BackdropFilter:
            return "BackdropFilter";

        case Grid::StylePropertyId::Transform:
            return "Transform";

        case Grid::StylePropertyId::Translate:
            return "Translate";

        case Grid::StylePropertyId::Rotate:
            return "Rotate";

        case Grid::StylePropertyId::Scale:
            return "Scale";

        case Grid::StylePropertyId::TransformOrigin:
            return "TransformOrigin";

        case Grid::StylePropertyId::MarginTop:
            return "MarginTop";

        case Grid::StylePropertyId::MarginBottom:
            return "MarginBottom";

        case Grid::StylePropertyId::MarginLeft:
            return "MarginLeft";

        case Grid::StylePropertyId::MarginRight:
            return "MarginRight";

        case Grid::StylePropertyId::PaddingTop:
            return "PaddingTop";

        case Grid::StylePropertyId::PaddingBottom:
            return "PaddingBottom";

        case Grid::StylePropertyId::PaddingLeft:
            return "PaddingLeft";

        case Grid::StylePropertyId::PaddingRight:
            return "PaddingRight";

        case Grid::StylePropertyId::BorderTopWidth:
            return "BorderTopWidth";

        case Grid::StylePropertyId::BorderRightWidth:
            return "BorderRightWidth";

        case Grid::StylePropertyId::BorderBottomWidth:
            return "BorderBottomWidth";

        case Grid::StylePropertyId::BorderLeftWidth:
            return "BorderLeftWidth";

        case Grid::StylePropertyId::BorderTopColor:
            return "BorderTopColor";

        case Grid::StylePropertyId::BorderRightColor:
            return "BorderRightColor";

        case Grid::StylePropertyId::BorderBottomColor:
            return "BorderBottomColor";

        case Grid::StylePropertyId::BorderLeftColor:
            return "BorderLeftColor";

        case Grid::StylePropertyId::GapTop:
            return "GapTop";

        case Grid::StylePropertyId::GapBottom:
            return "GapBottom";

        case Grid::StylePropertyId::GapLeft:
            return "GapLeft";

        case Grid::StylePropertyId::GapRight:
            return "GapRight";

        default:
            return "";
        }
    }
}
