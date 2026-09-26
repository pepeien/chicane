#include "Chicane/Grid/Style/Property/Table.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {

        static void readColor(const Color::Rgba& inValue, float* outValues)
        {
            outValues[0] = static_cast<float>(inValue.r);
            outValues[1] = static_cast<float>(inValue.g);
            outValues[2] = static_cast<float>(inValue.b);
            outValues[3] = static_cast<float>(inValue.a);
        }

        static Color::Rgba writeColor(const float* inValues)
        {
            auto channel = [](float inChannel) -> std::uint8_t
            { return static_cast<std::uint8_t>(std::round(std::clamp(inChannel, 0.0f, 255.0f))); };

            return Color::Rgba(channel(inValues[0]), channel(inValues[1]), channel(inValues[2]), channel(inValues[3]));
        }

        using Entries = std::array<StylePropertyEntry, StylePropertyTable::COUNT>;

        static Entries buildEntries()
        {
            Entries entries = {};

            auto set = [&entries](
                           StylePropertyId           inId,
                           const char*               inName,
                           std::uint8_t              inArity,
                           StylePropertyDirty        inDirty,
                           StylePropertyEntry::Read  inRead,
                           StylePropertyEntry::Write inWrite
                       )
            {
                StylePropertyEntry& entry = entries.at(static_cast<std::size_t>(inId));
                entry.name                = inName;
                entry.arity               = inArity;
                entry.dirty               = inDirty;
                entry.read                = inRead;
                entry.write               = inWrite;
            };

            set(
                StylePropertyId::Opacity,
                Style::OPACITY_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.opacity.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.opacity.set(inValues[0]); }
            );

            set(
                StylePropertyId::Width,
                Style::WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (inStyle.width.isAuto())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.width.value.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.width.value.set(inValues[0]); }
            );

            set(
                StylePropertyId::Height,
                Style::HEIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (inStyle.height.isAuto())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.height.value.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.height.value.set(inValues[0]); }
            );

            set(
                StylePropertyId::MinWidth,
                Style::MIN_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (!inStyle.width.hasMin())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.width.min.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.width.min.set(inValues[0]); }
            );

            set(
                StylePropertyId::MinHeight,
                Style::MIN_HEIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (!inStyle.height.hasMin())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.height.min.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.height.min.set(inValues[0]); }
            );

            set(
                StylePropertyId::MaxWidth,
                Style::MAX_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (!inStyle.width.hasMax())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.width.max.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.width.max.set(inValues[0]); }
            );

            set(
                StylePropertyId::MaxHeight,
                Style::MAX_HEIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    if (!inStyle.height.hasMax())
                    {
                        return false;
                    }

                    outValues[0] = inStyle.height.max.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.height.max.set(inValues[0]); }
            );

            set(
                StylePropertyId::ZIndex,
                Style::Z_INDEX_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.zIndex.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.zIndex.set(inValues[0]); }
            );

            set(
                StylePropertyId::ForegroundColor,
                Style::FOREGROUND_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint | StylePropertyDirty::Text,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.foregroundColor.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.foregroundColor.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::BackgroundColor,
                Style::BACKGROUND_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.background.color.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.background.color.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::FontSize,
                Style::FONT_SIZE_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout | StylePropertyDirty::Text,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.font.size.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.font.size.set(inValues[0]); }
            );

            set(
                StylePropertyId::LetterSpacing,
                Style::LETTER_SPACING_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout | StylePropertyDirty::Text,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.letterSpacing.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.letterSpacing.set(inValues[0]); }
            );

            set(
                StylePropertyId::Filter,
                Style::FILTER_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.filter.blur.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.filter.blur.set(inValues[0]); }
            );

            set(
                StylePropertyId::BackdropFilter,
                Style::BACKDROP_FILTER_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.backdrop.blur.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.backdrop.blur.set(inValues[0]); }
            );

            set(
                StylePropertyId::Transform,
                Style::TRANSFORM_ATTRIBUTE_NAME,
                5,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    const StyleTransform value = inStyle.transform.get();

                    outValues[0] = value.translation.x;
                    outValues[1] = value.translation.y;
                    outValues[2] = value.rotation;
                    outValues[3] = value.scale.x;
                    outValues[4] = value.scale.y;

                    return true;
                },
                [](Style& outStyle, const float* inValues)
                {
                    StyleTransform value = outStyle.transform.get();
                    value.translation.x  = inValues[0];
                    value.translation.y  = inValues[1];
                    value.rotation       = inValues[2];
                    value.scale.x        = inValues[3];
                    value.scale.y        = inValues[4];

                    outStyle.transform.set(value);
                }
            );

            set(
                StylePropertyId::Translate,
                Style::TRANSLATE_ATTRIBUTE_NAME,
                2,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    const Vec2 value = inStyle.translate.get();

                    outValues[0] = value.x;
                    outValues[1] = value.y;

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.translate.set(Vec2(inValues[0], inValues[1])); }
            );

            set(
                StylePropertyId::Rotate,
                Style::ROTATE_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.rotate.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.rotate.set(inValues[0]); }
            );

            set(
                StylePropertyId::Scale,
                Style::SCALE_ATTRIBUTE_NAME,
                2,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    const Vec2 value = inStyle.scale.get();

                    outValues[0] = value.x;
                    outValues[1] = value.y;

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.scale.set(Vec2(inValues[0], inValues[1])); }
            );

            set(
                StylePropertyId::TransformOrigin,
                Style::TRANSFORM_ORIGIN_ATTRIBUTE_NAME,
                2,
                StylePropertyDirty::Paint | StylePropertyDirty::Subtree,
                [](const Style& inStyle, float* outValues)
                {
                    if (inStyle.transformOrigin.getRaw().isEmpty())
                    {
                        return false;
                    }

                    const Vec2 value = inStyle.transformOrigin.get();

                    outValues[0] = value.x;
                    outValues[1] = value.y;

                    return true;
                },
                [](Style& outStyle, const float* inValues)
                { outStyle.transformOrigin.set(Vec2(inValues[0], inValues[1])); }
            );

            set(
                StylePropertyId::MarginTop,
                Style::MARGIN_TOP_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.margin.top.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.margin.top.set(inValues[0]); }
            );

            set(
                StylePropertyId::MarginBottom,
                Style::MARGIN_BOTTOM_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.margin.bottom.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.margin.bottom.set(inValues[0]); }
            );

            set(
                StylePropertyId::MarginLeft,
                Style::MARGIN_LEFT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.margin.left.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.margin.left.set(inValues[0]); }
            );

            set(
                StylePropertyId::MarginRight,
                Style::MARGIN_RIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.margin.right.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.margin.right.set(inValues[0]); }
            );

            set(
                StylePropertyId::PaddingTop,
                Style::PADDING_TOP_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.padding.top.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.padding.top.set(inValues[0]); }
            );

            set(
                StylePropertyId::PaddingBottom,
                Style::PADDING_BOTTOM_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.padding.bottom.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.padding.bottom.set(inValues[0]); }
            );

            set(
                StylePropertyId::PaddingLeft,
                Style::PADDING_LEFT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.padding.left.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.padding.left.set(inValues[0]); }
            );

            set(
                StylePropertyId::PaddingRight,
                Style::PADDING_RIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.padding.right.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.padding.right.set(inValues[0]); }
            );

            set(
                StylePropertyId::BorderTopWidth,
                Style::BORDER_TOP_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.border.width.top.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.width.top.set(inValues[0]); }
            );

            set(
                StylePropertyId::BorderRightWidth,
                Style::BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.border.width.right.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.width.right.set(inValues[0]); }
            );

            set(
                StylePropertyId::BorderBottomWidth,
                Style::BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.border.width.bottom.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.width.bottom.set(inValues[0]); }
            );

            set(
                StylePropertyId::BorderLeftWidth,
                Style::BORDER_LEFT_WIDTH_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.border.width.left.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.width.left.set(inValues[0]); }
            );

            set(
                StylePropertyId::BorderTopColor,
                Style::BORDER_TOP_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.border.colorTop.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.colorTop.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::BorderRightColor,
                Style::BORDER_RIGHT_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.border.colorRight.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.colorRight.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::BorderBottomColor,
                Style::BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.border.colorBottom.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.colorBottom.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::BorderLeftColor,
                Style::BORDER_LEFT_COLOR_ATTRIBUTE_NAME,
                4,
                StylePropertyDirty::Paint,
                [](const Style& inStyle, float* outValues)
                {
                    readColor(inStyle.border.colorLeft.get(), outValues);

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.border.colorLeft.set(writeColor(inValues)); }
            );

            set(
                StylePropertyId::GapTop,
                Style::GAP_TOP_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.gap.top.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.gap.top.set(inValues[0]); }
            );

            set(
                StylePropertyId::GapBottom,
                Style::GAP_BOTTOM_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.gap.bottom.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.gap.bottom.set(inValues[0]); }
            );

            set(
                StylePropertyId::GapLeft,
                Style::GAP_LEFT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.gap.left.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.gap.left.set(inValues[0]); }
            );

            set(
                StylePropertyId::GapRight,
                Style::GAP_RIGHT_ATTRIBUTE_NAME,
                1,
                StylePropertyDirty::Layout,
                [](const Style& inStyle, float* outValues)
                {
                    outValues[0] = inStyle.gap.right.get();

                    return true;
                },
                [](Style& outStyle, const float* inValues) { outStyle.gap.right.set(inValues[0]); }
            );

            return entries;
        }

        static const Entries& entries()
        {
            static const Entries result = buildEntries();

            return result;
        }

        using Offsets = std::array<std::size_t, StylePropertyTable::COUNT>;

        static const Offsets& offsets()
        {
            static const Offsets result = []()
            {
                Offsets     values = {};
                std::size_t cursor = 0;

                for (std::size_t i = 0; i < StylePropertyTable::COUNT; i++)
                {
                    values.at(i) = cursor;
                    cursor += entries().at(i).arity;
                }

                return values;
            }();

            return result;
        }

        const StylePropertyEntry& StylePropertyTable::sGet(StylePropertyId inId)
        {
            return entries().at(static_cast<std::size_t>(inId));
        }

        bool StylePropertyTable::sFind(const String& inName, StylePropertyId& outId)
        {
            for (std::size_t i = 0; i < COUNT; i++)
            {
                if (inName.equals(entries().at(i).name))
                {
                    outId = static_cast<StylePropertyId>(i);

                    return true;
                }
            }

            return false;
        }

        std::size_t StylePropertyTable::sOffset(StylePropertyId inId)
        {
            return offsets().at(static_cast<std::size_t>(inId));
        }
    }
}
