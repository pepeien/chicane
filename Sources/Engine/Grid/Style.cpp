#include "Chicane/Grid/Style.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

#include "Chicane/Box/Font.hpp"

#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        Style::Style(const StyleRuleset::Properties& inProperties, Component* inParent)
            : Style()
        {
            setParent(inParent);
            setProperties(inProperties);
        }

        Style::Style()
            : display(StyleDisplay::Block),
              zIndex(0.0f),
              width(0.0f),
              height(0.0f),
              flex({}),
              position(StylePosition::Relative),
              align(StyleAlignment::Start),
              margin({}),
              padding({}),
              border({}),
              gap({}),
              overflowX(StyleOverflow::Visible),
              overflowY(StyleOverflow::Visible),
              radius({}),
              background({}),
              foregroundColor(Color::toRgba(Color::TEXT_COLOR_WHITE)),
              opacity(OPACITY_DEFAULT_VALUE),
              filter({}),
              backdrop({}),
              transform({}),
              translate(Vec2::Zero()),
              rotate(0.0f),
              scale(Vec2::One()),
              transformOrigin(Vec2::Zero()),
              font({}),
              letterSpacing(0.0f),
              cursor(WindowCursor::Default),
              transitions({}),
              animation({}),
              m_parent(nullptr),
              m_snapshot({})
        {
            display.parseWith(
                [this](const String& inValue)
                {
                    const String value = parseText(inValue);

                    if (value.equals(DISPLAY_TYPE_NONE))
                    {
                        return StyleDisplay::None;
                    }

                    if (value.equals(DISPLAY_TYPE_FLEX))
                    {
                        return StyleDisplay::Flex;
                    }

                    if (value.equals(DISPLAY_TYPE_HIDDEN))
                    {
                        return StyleDisplay::Hidden;
                    }

                    return StyleDisplay::Block;
                }
            );

            zIndex.parseWith([this](const String& inValue) { return parseSize(inValue, SizeDirection::Horizontal); });

            width.parseWith([this](const String& inValue) { return parseSize(inValue, SizeDirection::Horizontal); });

            height.parseWith([this](const String& inValue) { return parseSize(inValue, SizeDirection::Vertical); });

            flex.parseWith(
                [this](const String& inValue)
                {
                    const String value = parseText(inValue);

                    if (value.equals(FLEX_DIRECTION_TYPE_COLUMN))
                    {
                        return StyleFlexDirection::Column;
                    }

                    return StyleFlexDirection::Row;
                },
                [this](const String& inValue)
                {
                    const String value = parseText(inValue);

                    if (value.equals(FLEX_WRAP_TYPE_WRAP))
                    {
                        return StyleFlexWrap::Wrap;
                    }

                    return StyleFlexWrap::NoWrap;
                }
            );

            position.parseWith(
                [this](const String& inValue)
                {
                    const String value = parseText(inValue);

                    if (value.equals(POSITION_TYPE_ABSOLUTE))
                    {
                        return StylePosition::Absolute;
                    }

                    return StylePosition::Relative;
                }
            );

            align.parseWith(
                [this](const String& inValue)
                {
                    std::vector<String> values;

                    for (const String& block : splitOneliner(inValue))
                    {
                        values.push_back(parseText(block));
                    }

                    if (values.empty())
                    {
                        return StyleAlignment::Start;
                    }

                    return toAlignment(values.at(0));
                }
            );

            margin.parseWith([this](const String& inValue, SizeDirection inDirection)
                             { return parseSize(inValue, inDirection); });

            padding.parseWith([this](const String& inValue, SizeDirection inDirection)
                              { return parseSize(inValue, inDirection); });

            border.parseWith(
                [this](const String& inValue, SizeDirection inDirection)
                {
                    const String value = parseText(inValue).trim().toLower();

                    if (value.equals(BORDER_WIDTH_TYPE_THIN))
                    {
                        return 1.0f;
                    }

                    if (value.equals(BORDER_WIDTH_TYPE_MEDIUM))
                    {
                        return 3.0f;
                    }

                    if (value.equals(BORDER_WIDTH_TYPE_THICK))
                    {
                        return 5.0f;
                    }

                    return parseSize(inValue, inDirection);
                },
                [this](const String& inValue) { return parseColor(inValue); },
                [this](const String& inValue) { return parseBorderType(inValue); }
            );

            gap.parseWith([this](const String& inValue, SizeDirection inDirection)
                          { return parseSize(inValue, inDirection); });

            overflowX.parseWith([this](const String& inValue) { return parseOverflow(inValue); });

            overflowY.parseWith([this](const String& inValue) { return parseOverflow(inValue); });

            radius.parseWith(
                [this](const String& inValue, SizeDirection inDirection)
                {
                    Vec2 box = hasParent() ? m_parent->getSize() : Vec2::Zero();

                    return parseSize(inValue, inDirection, &box);
                }
            );

            background.parseWith(
                [this](const String& inValue) { return parseColor(inValue); },
                [this](const String& inValue) { return parseText(inValue); }
            );

            foregroundColor.parseWith([this](const String& inValue) { return parseColor(inValue); });

            opacity.parseWith([this](const String& inValue) { return parseSize(inValue, SizeDirection::Horizontal); });

            filter.parseWith([this](const String& inValue) { return parseFilter(inValue); });

            backdrop.parseWith([this](const String& inValue) { return parseFilter(inValue); });

            transform.parseWith([this](const String& inValue) { return parseTransform(inValue); });

            translate.parseWith([this](const String& inValue) { return parseTranslation(inValue); });

            rotate.parseWith([this](const String& inValue) { return parseRotation(inValue); });

            scale.parseWith([this](const String& inValue) { return parseScale(inValue); });

            transformOrigin.parseWith([this](const String& inValue) { return parseTransformOrigin(inValue); });

            font.parseWith(
                [this](const String& inValue) { return parseText(inValue); },
                [this](const String& inValue) { return parseSize(inValue, SizeDirection::Vertical); },
                [this](const String& inValue) { return parseWeight(inValue); }
            );

            letterSpacing.parseWith([this](const String& inValue)
                                    { return parseSize(inValue, SizeDirection::Horizontal); });

            cursor.parseWith([this](const String& inValue) { return parseCursor(inValue); });
        }

        bool Style::isDisplay(StyleDisplay inValue) const
        {
            return display.get() == inValue;
        }

        bool Style::isPosition(StylePosition inValue) const
        {
            return position.get() == inValue;
        }

        bool Style::isPositioned() const
        {
            return !position.getRaw().isEmpty();
        }

        bool Style::isClippingOverflow() const
        {
            return overflowX.get() != StyleOverflow::Visible || overflowY.get() != StyleOverflow::Visible;
        }

        float Style::insetLeft() const
        {
            return padding.left.get() + border.paintedLeft();
        }

        float Style::insetRight() const
        {
            return padding.right.get() + border.paintedRight();
        }

        float Style::insetTop() const
        {
            return padding.top.get() + border.paintedTop();
        }

        float Style::insetBottom() const
        {
            return padding.bottom.get() + border.paintedBottom();
        }

        float Style::insetHorizontal() const
        {
            return insetLeft() + insetRight();
        }

        float Style::insetVertical() const
        {
            return insetTop() + insetBottom();
        }

        void Style::setProperties(const StyleRuleset::Properties& inProperties)
        {
            if (inProperties.find(DISPLAY_ATTRIBUTE_NAME) != inProperties.end())
            {
                display.setRaw(inProperties.at(DISPLAY_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Z_INDEX_ATTRIBUTE_NAME) != inProperties.end())
            {
                zIndex.setRaw(inProperties.at(Z_INDEX_ATTRIBUTE_NAME));
            }

            if (inProperties.find(WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.setRaw(inProperties.at(WIDTH_ATTRIBUTE_NAME));
            }
            else
            {
                width.setRaw(Size::AUTO_KEYWORD);
            }

            if (inProperties.find(HEIGHT_ATTRIBUTE_NAME) != inProperties.end())
            {
                height.setRaw(inProperties.at(HEIGHT_ATTRIBUTE_NAME));
            }
            else
            {
                height.setRaw(Size::AUTO_KEYWORD);
            }

            if (inProperties.find(POSITION_ATTRIBUTE_NAME) != inProperties.end())
            {
                position.setRaw(inProperties.at(POSITION_ATTRIBUTE_NAME));
            }

            if (inProperties.find(ALIGNMENT_ATTRIBUTE_NAME) != inProperties.end())
            {
                align.setRaw(inProperties.at(ALIGNMENT_ATTRIBUTE_NAME));
            }

            if (inProperties.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                foregroundColor.setRaw(inProperties.at(FOREGROUND_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(OPACITY_ATTRIBUTE_NAME) != inProperties.end())
            {
                opacity.setRaw(inProperties.at(OPACITY_ATTRIBUTE_NAME));
            }

            if (inProperties.find(LETTER_SPACING_ATTRIBUTE_NAME) != inProperties.end())
            {
                letterSpacing.setRaw(inProperties.at(LETTER_SPACING_ATTRIBUTE_NAME));
            }

            flex.setProperties(inProperties);

            background.setProperties(inProperties);

            font.setProperties(inProperties);

            filter.setProperties(inProperties, FILTER_ATTRIBUTE_NAME);

            backdrop.setProperties(inProperties, BACKDROP_FILTER_ATTRIBUTE_NAME);

            if (inProperties.find(TRANSFORM_ATTRIBUTE_NAME) != inProperties.end())
            {
                transform.setRaw(inProperties.at(TRANSFORM_ATTRIBUTE_NAME));
            }

            if (inProperties.find(TRANSLATE_ATTRIBUTE_NAME) != inProperties.end())
            {
                translate.setRaw(inProperties.at(TRANSLATE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(ROTATE_ATTRIBUTE_NAME) != inProperties.end())
            {
                rotate.setRaw(inProperties.at(ROTATE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(SCALE_ATTRIBUTE_NAME) != inProperties.end())
            {
                scale.setRaw(inProperties.at(SCALE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(TRANSFORM_ORIGIN_ATTRIBUTE_NAME) != inProperties.end())
            {
                transformOrigin.setRaw(inProperties.at(TRANSFORM_ORIGIN_ATTRIBUTE_NAME));
            }

            margin.setProperties(
                inProperties,
                Style::MARGIN_ATTRIBUTE_NAME,
                Style::MARGIN_TOP_ATTRIBUTE_NAME,
                Style::MARGIN_BOTTOM_ATTRIBUTE_NAME,
                Style::MARGIN_LEFT_ATTRIBUTE_NAME,
                Style::MARGIN_RIGHT_ATTRIBUTE_NAME
            );

            padding.setProperties(
                inProperties,
                Style::PADDING_ATTRIBUTE_NAME,
                Style::PADDING_TOP_ATTRIBUTE_NAME,
                Style::PADDING_BOTTOM_ATTRIBUTE_NAME,
                Style::PADDING_LEFT_ATTRIBUTE_NAME,
                Style::PADDING_RIGHT_ATTRIBUTE_NAME
            );

            border.setProperties(inProperties);

            gap.setProperties(
                inProperties,
                Style::GAP_ATTRIBUTE_NAME,
                Style::GAP_TOP_ATTRIBUTE_NAME,
                Style::GAP_BOTTOM_ATTRIBUTE_NAME,
                Style::GAP_LEFT_ATTRIBUTE_NAME,
                Style::GAP_RIGHT_ATTRIBUTE_NAME
            );

            radius.setProperties(
                inProperties,
                Style::BORDER_RADIUS_ATTRIBUTE_NAME,
                Style::BORDER_TOP_LEFT_RADIUS_ATTRIBUTE_NAME,
                Style::BORDER_TOP_RIGHT_RADIUS_ATTRIBUTE_NAME,
                Style::BORDER_BOTTOM_RIGHT_RADIUS_ATTRIBUTE_NAME,
                Style::BORDER_BOTTOM_LEFT_RADIUS_ATTRIBUTE_NAME
            );

            if (inProperties.find(OVERFLOW_ATTRIBUTE_NAME) != inProperties.end())
            {
                const std::vector<String> values = splitOneliner(inProperties.at(OVERFLOW_ATTRIBUTE_NAME));

                if (values.size() == 1)
                {
                    overflowX.setRaw(values.at(0));
                    overflowY.setRaw(values.at(0));
                }
                else if (values.size() >= 2)
                {
                    overflowX.setRaw(values.at(0));
                    overflowY.setRaw(values.at(1));
                }
            }

            if (inProperties.find(OVERFLOW_X_ATTRIBUTE_NAME) != inProperties.end())
            {
                overflowX.setRaw(inProperties.at(OVERFLOW_X_ATTRIBUTE_NAME));
            }

            if (inProperties.find(OVERFLOW_Y_ATTRIBUTE_NAME) != inProperties.end())
            {
                overflowY.setRaw(inProperties.at(OVERFLOW_Y_ATTRIBUTE_NAME));
            }

            if (inProperties.find(CURSOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                cursor.setRaw(inProperties.at(CURSOR_ATTRIBUTE_NAME));
            }
            else
            {
                cursor.setRaw("");
            }

            parseTransitions(inProperties);
            parseAnimation(inProperties);

            refresh();
        }

        void Style::copyValuesFrom(const Style& inStyle)
        {
            display.copyValue(inStyle.display);
            zIndex.copyValue(inStyle.zIndex);
            width.copyValue(inStyle.width);
            height.copyValue(inStyle.height);

            flex.direction.copyValue(inStyle.flex.direction);
            flex.wrap.copyValue(inStyle.flex.wrap);

            position.copyValue(inStyle.position);
            align.copyValue(inStyle.align);

            margin.top.copyValue(inStyle.margin.top);
            margin.bottom.copyValue(inStyle.margin.bottom);
            margin.left.copyValue(inStyle.margin.left);
            margin.right.copyValue(inStyle.margin.right);

            padding.top.copyValue(inStyle.padding.top);
            padding.bottom.copyValue(inStyle.padding.bottom);
            padding.left.copyValue(inStyle.padding.left);
            padding.right.copyValue(inStyle.padding.right);

            border.width.top.copyValue(inStyle.border.width.top);
            border.width.bottom.copyValue(inStyle.border.width.bottom);
            border.width.left.copyValue(inStyle.border.width.left);
            border.width.right.copyValue(inStyle.border.width.right);
            border.typeTop.copyValue(inStyle.border.typeTop);
            border.typeBottom.copyValue(inStyle.border.typeBottom);
            border.typeLeft.copyValue(inStyle.border.typeLeft);
            border.typeRight.copyValue(inStyle.border.typeRight);
            border.colorTop.copyValue(inStyle.border.colorTop);
            border.colorBottom.copyValue(inStyle.border.colorBottom);
            border.colorLeft.copyValue(inStyle.border.colorLeft);
            border.colorRight.copyValue(inStyle.border.colorRight);

            gap.top.copyValue(inStyle.gap.top);
            gap.bottom.copyValue(inStyle.gap.bottom);
            gap.left.copyValue(inStyle.gap.left);
            gap.right.copyValue(inStyle.gap.right);

            overflowX.copyValue(inStyle.overflowX);
            overflowY.copyValue(inStyle.overflowY);

            radius.x.top.copyValue(inStyle.radius.x.top);
            radius.x.bottom.copyValue(inStyle.radius.x.bottom);
            radius.x.left.copyValue(inStyle.radius.x.left);
            radius.x.right.copyValue(inStyle.radius.x.right);

            radius.y.top.copyValue(inStyle.radius.y.top);
            radius.y.bottom.copyValue(inStyle.radius.y.bottom);
            radius.y.left.copyValue(inStyle.radius.y.left);
            radius.y.right.copyValue(inStyle.radius.y.right);

            background.color.copyValue(inStyle.background.color);
            background.image.copyValue(inStyle.background.image);

            foregroundColor.copyValue(inStyle.foregroundColor);
            opacity.copyValue(inStyle.opacity);
            filter.blur.copyValue(inStyle.filter.blur);
            backdrop.blur.copyValue(inStyle.backdrop.blur);
            transform.copyValue(inStyle.transform);
            translate.copyValue(inStyle.translate);
            rotate.copyValue(inStyle.rotate);
            scale.copyValue(inStyle.scale);
            transformOrigin.copyValue(inStyle.transformOrigin);

            font.family.copyValue(inStyle.font.family);
            font.size.copyValue(inStyle.font.size);
            font.weight.copyValue(inStyle.font.weight);
            letterSpacing.copyValue(inStyle.letterSpacing);
            cursor.copyValue(inStyle.cursor);

            transitions = inStyle.transitions;
            animation   = inStyle.animation;
        }

        void Style::resetValues()
        {
            static const Style defaults;

            copyValuesFrom(defaults);
        }

        void Style::snapshot()
        {
            m_snapshot = extractAnimatedProperties();
        }

        void Style::restore()
        {
            for (const auto& [name, value] : m_snapshot)
            {
                if (value.empty())
                {
                    continue;
                }

                const StyleTransition* transition = findTransition(name);

                if (!transition || transition->duration <= 0.0f)
                {
                    continue;
                }

                applyAnimatedProperty(name, value);
            }
        }

        const Style::Properties& Style::getSnapshot() const
        {
            return m_snapshot;
        }

        Style::Properties Style::extractAnimatedProperties() const
        {
            Properties result;

            for (const String& name : ANIMATABLE_PROPERTIES)
            {
                const std::vector<float> values = extractAnimatedProperty(name);

                if (values.empty())
                {
                    continue;
                }

                result[name] = values;
            }

            return result;
        }

        std::vector<float> Style::extractAnimatedProperty(const String& inName) const
        {
            auto asColor = [](const Color::Rgba& inValue) -> std::vector<float>
            {
                return {
                    static_cast<float>(inValue.r),
                    static_cast<float>(inValue.g),
                    static_cast<float>(inValue.b),
                    static_cast<float>(inValue.a)
                };
            };

            if (inName.equals(OPACITY_ATTRIBUTE_NAME))
            {
                return {opacity.get()};
            }

            if (inName.equals(WIDTH_ATTRIBUTE_NAME))
            {
                if (width.getRaw().isEmpty() || width.isRaw(Size::AUTO_KEYWORD))
                {
                    return {};
                }

                return {width.get()};
            }

            if (inName.equals(HEIGHT_ATTRIBUTE_NAME))
            {
                if (height.getRaw().isEmpty() || height.isRaw(Size::AUTO_KEYWORD))
                {
                    return {};
                }

                return {height.get()};
            }

            if (inName.equals(Z_INDEX_ATTRIBUTE_NAME))
            {
                return {zIndex.get()};
            }

            if (inName.equals(FOREGROUND_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(foregroundColor.get());
            }

            if (inName.equals(BACKGROUND_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(background.color.get());
            }

            if (inName.equals(FONT_SIZE_ATTRIBUTE_NAME))
            {
                return {font.size.get()};
            }

            if (inName.equals(LETTER_SPACING_ATTRIBUTE_NAME))
            {
                return {letterSpacing.get()};
            }

            if (inName.equals(FILTER_ATTRIBUTE_NAME))
            {
                return {filter.blur.get()};
            }

            if (inName.equals(BACKDROP_FILTER_ATTRIBUTE_NAME))
            {
                return {backdrop.blur.get()};
            }

            if (inName.equals(TRANSFORM_ATTRIBUTE_NAME))
            {
                const StyleTransform value = transform.get();

                return {value.translation.x, value.translation.y, value.rotation, value.scale.x, value.scale.y};
            }

            if (inName.equals(TRANSLATE_ATTRIBUTE_NAME))
            {
                const Vec2 value = translate.get();

                return {value.x, value.y};
            }

            if (inName.equals(ROTATE_ATTRIBUTE_NAME))
            {
                return {rotate.get()};
            }

            if (inName.equals(SCALE_ATTRIBUTE_NAME))
            {
                const Vec2 value = scale.get();

                return {value.x, value.y};
            }

            if (inName.equals(TRANSFORM_ORIGIN_ATTRIBUTE_NAME))
            {
                if (transformOrigin.getRaw().isEmpty())
                {
                    return {};
                }

                const Vec2 value = transformOrigin.get();

                return {value.x, value.y};
            }

            if (inName.equals(MARGIN_TOP_ATTRIBUTE_NAME))
            {
                return {margin.top.get()};
            }

            if (inName.equals(MARGIN_BOTTOM_ATTRIBUTE_NAME))
            {
                return {margin.bottom.get()};
            }

            if (inName.equals(MARGIN_LEFT_ATTRIBUTE_NAME))
            {
                return {margin.left.get()};
            }

            if (inName.equals(MARGIN_RIGHT_ATTRIBUTE_NAME))
            {
                return {margin.right.get()};
            }

            if (inName.equals(PADDING_TOP_ATTRIBUTE_NAME))
            {
                return {padding.top.get()};
            }

            if (inName.equals(PADDING_BOTTOM_ATTRIBUTE_NAME))
            {
                return {padding.bottom.get()};
            }

            if (inName.equals(PADDING_LEFT_ATTRIBUTE_NAME))
            {
                return {padding.left.get()};
            }

            if (inName.equals(PADDING_RIGHT_ATTRIBUTE_NAME))
            {
                return {padding.right.get()};
            }

            if (inName.equals(BORDER_TOP_WIDTH_ATTRIBUTE_NAME))
            {
                return {border.width.top.get()};
            }

            if (inName.equals(BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME))
            {
                return {border.width.right.get()};
            }

            if (inName.equals(BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME))
            {
                return {border.width.bottom.get()};
            }

            if (inName.equals(BORDER_LEFT_WIDTH_ATTRIBUTE_NAME))
            {
                return {border.width.left.get()};
            }

            if (inName.equals(BORDER_TOP_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(border.colorTop.get());
            }

            if (inName.equals(BORDER_RIGHT_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(border.colorRight.get());
            }

            if (inName.equals(BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(border.colorBottom.get());
            }

            if (inName.equals(BORDER_LEFT_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(border.colorLeft.get());
            }

            if (inName.equals(GAP_TOP_ATTRIBUTE_NAME))
            {
                return {gap.top.get()};
            }

            if (inName.equals(GAP_BOTTOM_ATTRIBUTE_NAME))
            {
                return {gap.bottom.get()};
            }

            if (inName.equals(GAP_LEFT_ATTRIBUTE_NAME))
            {
                return {gap.left.get()};
            }

            if (inName.equals(GAP_RIGHT_ATTRIBUTE_NAME))
            {
                return {gap.right.get()};
            }

            return {};
        }

        void Style::applyAnimatedProperty(const String& inName, const std::vector<float>& inValue)
        {
            if (inValue.empty())
            {
                return;
            }

            auto asColor = [](const std::vector<float>& inChannels) -> Color::Rgba
            {
                auto channel = [](float inChannel) -> std::uint8_t
                { return static_cast<std::uint8_t>(std::round(std::clamp(inChannel, 0.0f, 255.0f))); };

                if (inChannels.size() < 4)
                {
                    return Color::Rgba(0U, 0U, 0U, 0U);
                }

                return Color::Rgba(
                    channel(inChannels.at(0)),
                    channel(inChannels.at(1)),
                    channel(inChannels.at(2)),
                    channel(inChannels.at(3))
                );
            };

            if (inName.equals(OPACITY_ATTRIBUTE_NAME))
            {
                opacity.set(inValue.at(0));

                return;
            }

            if (inName.equals(WIDTH_ATTRIBUTE_NAME))
            {
                width.set(inValue.at(0));

                return;
            }

            if (inName.equals(HEIGHT_ATTRIBUTE_NAME))
            {
                height.set(inValue.at(0));

                return;
            }

            if (inName.equals(Z_INDEX_ATTRIBUTE_NAME))
            {
                zIndex.set(inValue.at(0));

                return;
            }

            if (inName.equals(FOREGROUND_COLOR_ATTRIBUTE_NAME))
            {
                foregroundColor.set(asColor(inValue));

                return;
            }

            if (inName.equals(BACKGROUND_COLOR_ATTRIBUTE_NAME))
            {
                background.color.set(asColor(inValue));

                return;
            }

            if (inName.equals(FONT_SIZE_ATTRIBUTE_NAME))
            {
                font.size.set(inValue.at(0));

                return;
            }

            if (inName.equals(LETTER_SPACING_ATTRIBUTE_NAME))
            {
                letterSpacing.set(inValue.at(0));

                return;
            }

            if (inName.equals(FILTER_ATTRIBUTE_NAME))
            {
                filter.blur.set(inValue.at(0));

                return;
            }

            if (inName.equals(BACKDROP_FILTER_ATTRIBUTE_NAME))
            {
                backdrop.blur.set(inValue.at(0));

                return;
            }

            if (inName.equals(TRANSFORM_ATTRIBUTE_NAME))
            {
                StyleTransform value = transform.get();

                if (inValue.size() > 0)
                {
                    value.translation.x = inValue.at(0);
                }

                if (inValue.size() > 1)
                {
                    value.translation.y = inValue.at(1);
                }

                if (inValue.size() > 2)
                {
                    value.rotation = inValue.at(2);
                }

                if (inValue.size() > 3)
                {
                    value.scale.x = inValue.at(3);
                }

                if (inValue.size() > 4)
                {
                    value.scale.y = inValue.at(4);
                }

                transform.set(value);

                return;
            }

            if (inName.equals(TRANSLATE_ATTRIBUTE_NAME))
            {
                Vec2 value = translate.get();

                if (inValue.size() > 0)
                {
                    value.x = inValue.at(0);
                }

                if (inValue.size() > 1)
                {
                    value.y = inValue.at(1);
                }

                translate.set(value);

                return;
            }

            if (inName.equals(ROTATE_ATTRIBUTE_NAME))
            {
                rotate.set(inValue.at(0));

                return;
            }

            if (inName.equals(SCALE_ATTRIBUTE_NAME))
            {
                Vec2 value = scale.get();

                if (inValue.size() > 0)
                {
                    value.x = inValue.at(0);
                    value.y = inValue.at(0);
                }

                if (inValue.size() > 1)
                {
                    value.y = inValue.at(1);
                }

                scale.set(value);

                return;
            }

            if (inName.equals(TRANSFORM_ORIGIN_ATTRIBUTE_NAME))
            {
                Vec2 value = transformOrigin.get();

                if (inValue.size() > 0)
                {
                    value.x = inValue.at(0);
                }

                if (inValue.size() > 1)
                {
                    value.y = inValue.at(1);
                }

                transformOrigin.set(value);

                return;
            }

            if (inName.equals(MARGIN_TOP_ATTRIBUTE_NAME))
            {
                margin.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(MARGIN_BOTTOM_ATTRIBUTE_NAME))
            {
                margin.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(MARGIN_LEFT_ATTRIBUTE_NAME))
            {
                margin.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(MARGIN_RIGHT_ATTRIBUTE_NAME))
            {
                margin.right.set(inValue.at(0));

                return;
            }

            if (inName.equals(PADDING_TOP_ATTRIBUTE_NAME))
            {
                padding.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(PADDING_BOTTOM_ATTRIBUTE_NAME))
            {
                padding.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(PADDING_LEFT_ATTRIBUTE_NAME))
            {
                padding.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(PADDING_RIGHT_ATTRIBUTE_NAME))
            {
                padding.right.set(inValue.at(0));

                return;
            }

            if (inName.equals(BORDER_TOP_WIDTH_ATTRIBUTE_NAME))
            {
                border.width.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME))
            {
                border.width.right.set(inValue.at(0));

                return;
            }

            if (inName.equals(BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME))
            {
                border.width.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(BORDER_LEFT_WIDTH_ATTRIBUTE_NAME))
            {
                border.width.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(BORDER_TOP_COLOR_ATTRIBUTE_NAME))
            {
                border.colorTop.set(asColor(inValue));

                return;
            }

            if (inName.equals(BORDER_RIGHT_COLOR_ATTRIBUTE_NAME))
            {
                border.colorRight.set(asColor(inValue));

                return;
            }

            if (inName.equals(BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME))
            {
                border.colorBottom.set(asColor(inValue));

                return;
            }

            if (inName.equals(BORDER_LEFT_COLOR_ATTRIBUTE_NAME))
            {
                border.colorLeft.set(asColor(inValue));

                return;
            }

            if (inName.equals(GAP_TOP_ATTRIBUTE_NAME))
            {
                gap.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(GAP_BOTTOM_ATTRIBUTE_NAME))
            {
                gap.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(GAP_LEFT_ATTRIBUTE_NAME))
            {
                gap.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(GAP_RIGHT_ATTRIBUTE_NAME))
            {
                gap.right.set(inValue.at(0));
            }
        }

        const StyleTransition* Style::findTransition(const String& inName) const
        {
            auto contains = [](const String& inProperty, const String& inTarget) -> bool
            {
                if (inProperty.equals(inTarget))
                {
                    return true;
                }

                if (inProperty.equals(PADDING_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(PADDING_TOP_ATTRIBUTE_NAME) ||
                           inTarget.equals(PADDING_RIGHT_ATTRIBUTE_NAME) ||
                           inTarget.equals(PADDING_BOTTOM_ATTRIBUTE_NAME) ||
                           inTarget.equals(PADDING_LEFT_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(MARGIN_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(MARGIN_TOP_ATTRIBUTE_NAME) || inTarget.equals(MARGIN_RIGHT_ATTRIBUTE_NAME) ||
                           inTarget.equals(MARGIN_BOTTOM_ATTRIBUTE_NAME) || inTarget.equals(MARGIN_LEFT_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(GAP_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(GAP_TOP_ATTRIBUTE_NAME) || inTarget.equals(GAP_RIGHT_ATTRIBUTE_NAME) ||
                           inTarget.equals(GAP_BOTTOM_ATTRIBUTE_NAME) || inTarget.equals(GAP_LEFT_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_WIDTH_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_TOP_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_LEFT_WIDTH_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_COLOR_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_TOP_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_RIGHT_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_LEFT_COLOR_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_TOP_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_TOP_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_TOP_COLOR_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_RIGHT_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_RIGHT_COLOR_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_BOTTOM_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_LEFT_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_LEFT_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_LEFT_COLOR_ATTRIBUTE_NAME);
                }

                if (inProperty.equals(BORDER_ATTRIBUTE_NAME))
                {
                    return inTarget.equals(BORDER_TOP_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_LEFT_WIDTH_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_TOP_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_RIGHT_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME) ||
                           inTarget.equals(BORDER_LEFT_COLOR_ATTRIBUTE_NAME);
                }

                return false;
            };

            const StyleTransition* all       = nullptr;
            const StyleTransition* shorthand = nullptr;
            const StyleTransition* specific  = nullptr;

            for (const StyleTransition& transition : transitions)
            {
                if (transition.property.equals(TRANSITION_PROPERTY_ALL))
                {
                    all = &transition;
                }

                if (transition.property.equals(inName))
                {
                    specific = &transition;
                }
                else if (contains(transition.property, inName))
                {
                    shorthand = &transition;
                }
            }

            return specific ? specific : (shorthand ? shorthand : all);
        }

        bool Style::hasParent() const
        {
            return m_parent != nullptr;
        }

        void Style::setParent(const Component* inComponent)
        {
            if (m_parent == inComponent)
            {
                return;
            }

            m_parent = inComponent;

            refresh();
        }

        void Style::refresh()
        {
            refreshDisplay();
            refreshFlex();
            refreshZIndex();
            refreshSize();
            refreshPosition();
            refreshMargin();
            refreshPadding();
            refreshBorder();
            refreshGap();
            refreshOverflow();
            refreshRadius();
            refreshAlignment();
            refreshBackground();
            refreshForegroundColor();
            refreshOpacity();
            refreshFilter();
            refreshTransform();
            refreshFont();
            refreshLetterSpacing();
            refreshCursor();
        }

        void Style::refreshDisplay()
        {
            display.refresh();
        }

        void Style::refreshFlex()
        {
            flex.refresh();
        }

        void Style::refreshZIndex()
        {
            zIndex.refresh();
        }

        void Style::refreshSize()
        {
            width.refresh();
            height.refresh();
        }

        void Style::refreshPosition()
        {
            position.refresh();
        }

        void Style::refreshAlignment()
        {
            align.refresh();
        }

        void Style::refreshMargin()
        {
            margin.refresh();
        }

        void Style::refreshPadding()
        {
            padding.refresh();
        }

        void Style::refreshBorder()
        {
            border.refresh();
        }

        void Style::refreshGap()
        {
            gap.refresh();
        }

        void Style::refreshOverflow()
        {
            overflowX.refresh();
            overflowY.refresh();

            if (overflowX.get() == StyleOverflow::Visible && overflowY.get() != StyleOverflow::Visible)
            {
                overflowX.set(StyleOverflow::Auto);
            }

            if (overflowY.get() == StyleOverflow::Visible && overflowX.get() != StyleOverflow::Visible)
            {
                overflowY.set(StyleOverflow::Auto);
            }
        }

        void Style::refreshRadius()
        {
            radius.refresh();

            if (hasParent())
            {
                const Vec2& size = m_parent->getSize();
                radius.constrain(size.x, size.y);
            }
        }

        StyleOverflow Style::parseOverflow(const String& inValue) const
        {
            const String value = parseText(inValue);

            if (value.equals(OVERFLOW_TYPE_HIDDEN))
            {
                return StyleOverflow::Hidden;
            }

            if (value.equals(OVERFLOW_TYPE_SCROLL))
            {
                return StyleOverflow::Scroll;
            }

            if (value.equals(OVERFLOW_TYPE_AUTO))
            {
                return StyleOverflow::Auto;
            }

            return StyleOverflow::Visible;
        }

        StyleBorderType Style::parseBorderType(const String& inValue) const
        {
            const String value = parseText(inValue).trim().toLower();

            if (value.equals(BORDER_STYLE_TYPE_SOLID) || value.equals(BORDER_STYLE_TYPE_DASHED) ||
                value.equals(BORDER_STYLE_TYPE_DOTTED) || value.equals(BORDER_STYLE_TYPE_DOUBLE) ||
                value.equals(BORDER_STYLE_TYPE_GROOVE) || value.equals(BORDER_STYLE_TYPE_RIDGE) ||
                value.equals(BORDER_STYLE_TYPE_INSET) || value.equals(BORDER_STYLE_TYPE_OUTSET))
            {
                return StyleBorderType::Solid;
            }

            return StyleBorderType::None;
        }

        void Style::refreshBackground()
        {
            background.refresh();
        }

        void Style::refreshForegroundColor()
        {
            foregroundColor.refresh();
        }

        void Style::refreshOpacity()
        {
            opacity.refresh();
        }

        void Style::refreshFilter()
        {
            filter.refresh();
            backdrop.refresh();
        }

        void Style::refreshTransform()
        {
            transform.refresh();
            translate.refresh();
            rotate.refresh();
            scale.refresh();
            transformOrigin.refresh();
        }

        StyleTransform Style::getTransform() const
        {
            StyleTransform result           = transform.get();
            const Vec2     extraTranslation = translate.get();
            const Vec2     extraScale       = scale.get();

            result.translation.x += extraTranslation.x;
            result.translation.y += extraTranslation.y;
            result.rotation += rotate.get();
            result.scale.x *= extraScale.x;
            result.scale.y *= extraScale.y;

            return result;
        }

        Vec2 Style::getTransformOrigin() const
        {
            return getTransformOrigin(hasParent() ? m_parent->getSize() : Vec2::Zero());
        }

        Vec2 Style::getTransformOrigin(const Vec2& inBox) const
        {
            if (transformOrigin.getRaw().isEmpty())
            {
                return Vec2(inBox.x * 0.5f, inBox.y * 0.5f);
            }

            return parseTransformOrigin(transformOrigin.getRaw(), inBox);
        }

        float Style::parseFilter(const String& inValue) const
        {
            const String value = parseText(inValue).trim();

            if (value.isEmpty() || value.equals(FILTER_TYPE_NONE))
            {
                return 0.0f;
            }

            float blur = 0.0f;

            for (const String& block : splitOneliner(value))
            {
                const String token = block.trim();

                if (!token.startsWith(FILTER_BLUR_KEYWORD) || !token.contains(METHOD_PARAMS_OPENING))
                {
                    continue;
                }

                const String argument = token.getBetween(METHOD_PARAMS_OPENING, METHOD_PARAMS_CLOSING).trim();

                if (argument.isEmpty() || argument.contains(Size::PERCENTAGE_UNIT))
                {
                    continue;
                }

                const float radius = std::max(0.0f, parseSize(argument, SizeDirection::Horizontal));

                blur = std::sqrt((blur * blur) + (radius * radius));
            }

            return blur;
        }

        StyleTransform Style::parseTransform(const String& inValue) const
        {
            StyleTransform result;
            const String   value = parseText(inValue).trim();

            if (value.isEmpty() || value.equals(TRANSFORM_TYPE_NONE))
            {
                return result;
            }

            for (const String& block : splitOneliner(value))
            {
                const String token = block.trim();

                if (!token.contains(METHOD_PARAMS_OPENING))
                {
                    continue;
                }

                const String argument          = token.getBetween(METHOD_PARAMS_OPENING, METHOD_PARAMS_CLOSING).trim();
                const std::vector<String> args = argument.split(METHOD_PARAMS_SEPARATOR);

                if (token.startsWith(TRANSFORM_TRANSLATE_X_KEYWORD))
                {
                    if (!args.empty())
                    {
                        result.translation.x += parseSize(args.at(0).trim(), SizeDirection::Horizontal);
                    }

                    continue;
                }

                if (token.startsWith(TRANSFORM_TRANSLATE_Y_KEYWORD))
                {
                    if (!args.empty())
                    {
                        result.translation.y += parseSize(args.at(0).trim(), SizeDirection::Vertical);
                    }

                    continue;
                }

                if (token.startsWith(TRANSFORM_TRANSLATE_KEYWORD))
                {
                    const Vec2 translation = parseTranslation(argument);

                    result.translation.x += translation.x;
                    result.translation.y += translation.y;

                    continue;
                }

                if (token.startsWith(TRANSFORM_ROTATE_KEYWORD))
                {
                    result.rotation += parseRotation(argument);

                    continue;
                }

                if (token.startsWith(TRANSFORM_SCALE_X_KEYWORD))
                {
                    if (!args.empty())
                    {
                        result.scale.x *= parseScale(args.at(0).trim()).x;
                    }

                    continue;
                }

                if (token.startsWith(TRANSFORM_SCALE_Y_KEYWORD))
                {
                    if (!args.empty())
                    {
                        result.scale.y *= parseScale(args.at(0).trim()).y;
                    }

                    continue;
                }

                if (token.startsWith(TRANSFORM_SCALE_KEYWORD))
                {
                    const Vec2 scaleValue = parseScale(argument);

                    result.scale.x *= scaleValue.x;
                    result.scale.y *= scaleValue.y;
                }
            }

            return result;
        }

        Vec2 Style::parseTranslation(const String& inValue) const
        {
            const String value = parseText(inValue).trim();

            if (value.isEmpty() || value.equals(TRANSFORM_TYPE_NONE))
            {
                return Vec2::Zero();
            }

            std::vector<String> tokens = value.split(METHOD_PARAMS_SEPARATOR);

            if (tokens.size() < 2)
            {
                tokens = splitOneliner(value);
            }

            if (tokens.empty())
            {
                return Vec2::Zero();
            }

            const float x = parseSize(tokens.at(0).trim(), SizeDirection::Horizontal);
            const float y = tokens.size() > 1 ? parseSize(tokens.at(1).trim(), SizeDirection::Vertical) : 0.0f;

            return {x, y};
        }

        float Style::parseRotation(const String& inValue) const
        {
            const String value = parseText(inValue).trim();

            if (value.isEmpty() || value.equals(TRANSFORM_TYPE_NONE))
            {
                return 0.0f;
            }

            if (value.endsWith(TRANSFORM_DEGREE_UNIT))
            {
                return parseSize(value.substr(0, value.size() - 3), SizeDirection::Horizontal);
            }

            if (value.endsWith(TRANSFORM_RADIAN_UNIT))
            {
                return parseSize(value.substr(0, value.size() - 3), SizeDirection::Horizontal) * (180.0f / 3.14159265f);
            }

            if (value.endsWith(TRANSFORM_TURN_UNIT))
            {
                return parseSize(value.substr(0, value.size() - 4), SizeDirection::Horizontal) * 360.0f;
            }

            return parseSize(value, SizeDirection::Horizontal);
        }

        Vec2 Style::parseScale(const String& inValue) const
        {
            const String value = parseText(inValue).trim();

            if (value.isEmpty() || value.equals(TRANSFORM_TYPE_NONE))
            {
                return Vec2::One();
            }

            auto parseFactor = [this](const String& inFactor, SizeDirection inDirection) -> float
            {
                const String factor = inFactor.trim();

                if (factor.endsWith(Size::PERCENTAGE_UNIT))
                {
                    return parseSize(factor.substr(0, factor.size() - 1), inDirection) / 100.0f;
                }

                return parseSize(factor, inDirection);
            };

            const std::vector<String> args = value.split(METHOD_PARAMS_SEPARATOR);

            if (args.size() >= 2)
            {
                return {
                    parseFactor(args.at(0), SizeDirection::Horizontal),
                    parseFactor(args.at(1), SizeDirection::Vertical)
                };
            }

            const std::vector<String> tokens = splitOneliner(value);

            if (tokens.size() >= 2)
            {
                return {
                    parseFactor(tokens.at(0), SizeDirection::Horizontal),
                    parseFactor(tokens.at(1), SizeDirection::Vertical)
                };
            }

            if (!args.empty() && !args.at(0).trim().isEmpty())
            {
                const float factor = parseFactor(args.at(0), SizeDirection::Horizontal);

                return {factor, factor};
            }

            if (!tokens.empty())
            {
                const float factor = parseFactor(tokens.at(0), SizeDirection::Horizontal);

                return {factor, factor};
            }

            return Vec2::One();
        }

        Vec2 Style::parseTransformOrigin(const String& inValue) const
        {
            return parseTransformOrigin(inValue, hasParent() ? m_parent->getSize() : Vec2::Zero());
        }

        Vec2 Style::parseTransformOrigin(const String& inValue, const Vec2& inBox) const
        {
            const String value   = parseText(inValue).trim();
            const float  centerX = inBox.x * 0.5f;
            const float  centerY = inBox.y * 0.5f;

            if (value.isEmpty())
            {
                return {centerX, centerY};
            }

            std::vector<String> tokens;

            for (const String& block : splitOneliner(value))
            {
                const String token = block.trim();

                if (!token.isEmpty())
                {
                    tokens.push_back(token);
                }
            }

            if (tokens.empty())
            {
                return {centerX, centerY};
            }

            auto isLeft   = [](const String& inToken) { return inToken.equals(TRANSFORM_ORIGIN_TYPE_LEFT); };
            auto isRight  = [](const String& inToken) { return inToken.equals(TRANSFORM_ORIGIN_TYPE_RIGHT); };
            auto isTop    = [](const String& inToken) { return inToken.equals(TRANSFORM_ORIGIN_TYPE_TOP); };
            auto isBottom = [](const String& inToken) { return inToken.equals(TRANSFORM_ORIGIN_TYPE_BOTTOM); };
            auto isCenter = [](const String& inToken) { return inToken.equals(TRANSFORM_ORIGIN_TYPE_CENTER); };

            auto parseAxis = [&](const String& inToken, SizeDirection inDirection) -> float
            {
                if (isLeft(inToken) || isTop(inToken))
                {
                    return 0.0f;
                }

                if (isCenter(inToken))
                {
                    return inDirection == SizeDirection::Horizontal ? centerX : centerY;
                }

                if (isRight(inToken))
                {
                    return inBox.x;
                }

                if (isBottom(inToken))
                {
                    return inBox.y;
                }

                return parseSize(inToken, inDirection, &inBox);
            };

            if (tokens.size() == 1)
            {
                const String& token = tokens.at(0);

                if (isTop(token) || isBottom(token))
                {
                    return {centerX, parseAxis(token, SizeDirection::Vertical)};
                }

                return {parseAxis(token, SizeDirection::Horizontal), centerY};
            }

            String xToken = tokens.at(0);
            String yToken = tokens.at(1);

            const bool firstIsTopBottom  = isTop(xToken) || isBottom(xToken);
            const bool secondIsTopBottom = isTop(yToken) || isBottom(yToken);
            const bool firstIsLeftRight  = isLeft(xToken) || isRight(xToken);
            const bool secondIsLeftRight = isLeft(yToken) || isRight(yToken);

            if ((firstIsTopBottom && !secondIsTopBottom) ||
                (secondIsLeftRight && !firstIsLeftRight && !firstIsTopBottom))
            {
                std::swap(xToken, yToken);
            }

            return {parseAxis(xToken, SizeDirection::Horizontal), parseAxis(yToken, SizeDirection::Vertical)};
        }

        void Style::refreshFont()
        {
            font.refresh();
        }

        void Style::refreshLetterSpacing()
        {
            letterSpacing.refresh();
        }

        void Style::refreshCursor()
        {
            if (cursor.getRaw().isEmpty())
            {
                cursor.set(hasParent() ? m_parent->getStyle().cursor.get() : WindowCursor::Default);

                return;
            }

            cursor.refresh();
        }

        WindowCursor Style::parseCursor(const String& inValue) const
        {
            const String value = parseText(inValue).trim().toLower();

            if (value.equals(CURSOR_TYPE_POINTER))
            {
                return WindowCursor::Pointer;
            }

            if (value.equals(CURSOR_TYPE_TEXT))
            {
                return WindowCursor::Text;
            }

            if (value.equals(CURSOR_TYPE_CROSSHAIR))
            {
                return WindowCursor::Crosshair;
            }

            if (value.equals(CURSOR_TYPE_MOVE))
            {
                return WindowCursor::Move;
            }

            if (value.equals(CURSOR_TYPE_NOT_ALLOWED))
            {
                return WindowCursor::NotAllowed;
            }

            if (value.equals(CURSOR_TYPE_WAIT))
            {
                return WindowCursor::Wait;
            }

            if (value.equals(CURSOR_TYPE_PROGRESS))
            {
                return WindowCursor::Progress;
            }

            if (value.equals(CURSOR_TYPE_GRAB))
            {
                return WindowCursor::Grab;
            }

            if (value.equals(CURSOR_TYPE_GRABBING))
            {
                return WindowCursor::Grabbing;
            }

            if (value.equals(CURSOR_TYPE_NS_RESIZE))
            {
                return WindowCursor::NsResize;
            }

            if (value.equals(CURSOR_TYPE_EW_RESIZE))
            {
                return WindowCursor::EwResize;
            }

            if (value.equals(CURSOR_TYPE_NESW_RESIZE))
            {
                return WindowCursor::NeswResize;
            }

            if (value.equals(CURSOR_TYPE_NWSE_RESIZE))
            {
                return WindowCursor::NwseResize;
            }

            if (value.equals(CURSOR_TYPE_NONE))
            {
                return WindowCursor::None;
            }

            return WindowCursor::Default;
        }

        Color::Rgba Style::parseColor(const String& inValue) const
        {
            String result = "";

            if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
            {
                const String keyword = inValue.startsWith(RGBA_KEYWORD) ? RGBA_KEYWORD : RGB_KEYWORD;

                const std::vector<String> params =
                    inValue.getBetween(METHOD_PARAMS_OPENING, METHOD_PARAMS_CLOSING).split(METHOD_PARAMS_SEPARATOR);

                if (params.empty())
                {
                    throw std::runtime_error("Invalid " + keyword + " parameters");
                }

                result.append(keyword);
                result.append(METHOD_PARAMS_OPENING);
                for (const String& param : params)
                {
                    result.append(parseText(param.trim()));
                    result.append(",");
                }
                result.popBack();
                result.append(METHOD_PARAMS_CLOSING);
            }
            else
            {
                result = parseText(inValue);
            }

            return Color::toRgba(result);
        }

        float Style::parseSize(const String& inValue, SizeDirection inDirection, const Vec2* inBox) const
        {
            Size result;
            result.setFontSize(Box::Font::BASE_SIZE);
            result.setTextParser([this](const String& inValue) { return parseText(inValue); });

            if (hasParent() && m_parent->hasRoot())
            {
                result.setRoot(m_parent->getRoot()->getSize());
            }

            if (inBox)
            {
                result.setParent(*inBox);
            }
            else if (hasParent())
            {
                const Component* containingBlock = m_parent->getContainingBlock();
                if (containingBlock && containingBlock != m_parent)
                {
                    result.setParent(containingBlock->getSize());
                }
            }

            return result.parse(inValue, inDirection);
        }

        float Style::parseWeight(const String& inValue) const
        {
            const String value = parseText(inValue).trim().toLower();

            if (value.isEmpty() || value.equals(FONT_WEIGHT_TYPE_NORMAL))
            {
                return FONT_WEIGHT_NORMAL_VALUE;
            }

            if (value.equals(FONT_WEIGHT_TYPE_BOLD))
            {
                return FONT_WEIGHT_BOLD_VALUE;
            }

            const float inherited = hasParent() ? m_parent->getStyle().font.weight.get() : FONT_WEIGHT_NORMAL_VALUE;

            if (value.equals(FONT_WEIGHT_TYPE_BOLDER))
            {
                if (inherited <= 300.0f)
                {
                    return FONT_WEIGHT_NORMAL_VALUE;
                }

                if (inherited <= 500.0f)
                {
                    return FONT_WEIGHT_BOLD_VALUE;
                }

                return 900.0f;
            }

            if (value.equals(FONT_WEIGHT_TYPE_LIGHTER))
            {
                if (inherited <= FONT_WEIGHT_NORMAL_VALUE)
                {
                    return 100.0f;
                }

                if (inherited <= FONT_WEIGHT_BOLD_VALUE)
                {
                    return FONT_WEIGHT_NORMAL_VALUE;
                }

                return FONT_WEIGHT_BOLD_VALUE;
            }

            if (value.isNaN())
            {
                return FONT_WEIGHT_NORMAL_VALUE;
            }

            try
            {
                return std::clamp(std::stof(value.toStandard()), 1.0f, 1000.0f);
            }
            catch (const std::exception&)
            {
                return FONT_WEIGHT_NORMAL_VALUE;
            }
        }

        String Style::parseReference(const String& inValue) const
        {
            const std::uint32_t start = inValue.firstOf(METHOD_PARAMS_OPENING) + 1;
            const std::uint32_t end   = inValue.lastOf(METHOD_PARAMS_CLOSING);

            String result = "";
            result.append(REFERENCE_VALUE_OPENING);
            result.append(inValue.substr(start, end - start));
            result.append(REFERENCE_VALUE_CLOSING);
            result.append(inValue.substr(end + 1));

            return result;
        }

        String Style::parseText(const String& inValue) const
        {
            if (!hasParent())
            {
                return inValue;
            }

            String value = inValue.trim();

            if (value.startsWith(VARIABLE_KEYWORD))
            {
                value = parseText(m_parent->getStyleVariable(value.substr(1)));
            }

            if (value.startsWith(REFERENCE_KEYWORD))
            {
                value = parseReference(value);
            }

            return m_parent->parseText(value);
        }

        void Style::parseTransitions(const StyleRuleset::Properties& inProperties)
        {
            const bool bHasShorthand = inProperties.find(TRANSITION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasProperty  = inProperties.find(TRANSITION_PROPERTY_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasDuration  = inProperties.find(TRANSITION_DURATION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasEasing = inProperties.find(TRANSITION_TIMING_FUNCTION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasDelay  = inProperties.find(TRANSITION_DELAY_ATTRIBUTE_NAME) != inProperties.end();

            if (!bHasShorthand && !bHasProperty && !bHasDuration && !bHasEasing && !bHasDelay)
            {
                return;
            }

            transitions.clear();

            if (bHasShorthand)
            {
                for (const String& item : splitStyleList(inProperties.at(TRANSITION_ATTRIBUTE_NAME)))
                {
                    StyleTransition transition;
                    bool            bHasDurationToken = false;

                    for (const String& token : splitOneliner(item))
                    {
                        if (isTime(token))
                        {
                            if (!bHasDurationToken)
                            {
                                transition.duration = parseTime(token);
                                bHasDurationToken   = true;

                                continue;
                            }

                            transition.delay = parseTime(token);

                            continue;
                        }

                        if (isEasing(token))
                        {
                            transition.easing = parseEasing(token);

                            continue;
                        }

                        transition.property = token.trim();
                    }

                    if (!transition.property.isEmpty())
                    {
                        transitions.push_back(transition);
                    }
                }
            }

            auto applyList = [&](const String& inKey, auto&& inSetter)
            {
                if (inProperties.find(inKey) == inProperties.end())
                {
                    return;
                }

                const std::vector<String> values = splitStyleList(inProperties.at(inKey));

                if (values.empty())
                {
                    return;
                }

                if (transitions.empty())
                {
                    transitions.push_back(StyleTransition());
                }

                for (std::size_t i = 0; i < transitions.size(); i++)
                {
                    inSetter(transitions.at(i), values.at(std::min(i, values.size() - 1)));
                }
            };

            applyList(
                TRANSITION_PROPERTY_ATTRIBUTE_NAME,
                [](StyleTransition& outTransition, const String& inValue) { outTransition.property = inValue.trim(); }
            );
            applyList(
                TRANSITION_DURATION_ATTRIBUTE_NAME,
                [](StyleTransition& outTransition, const String& inValue)
                { outTransition.duration = parseTime(inValue); }
            );
            applyList(
                TRANSITION_TIMING_FUNCTION_ATTRIBUTE_NAME,
                [](StyleTransition& outTransition, const String& inValue)
                { outTransition.easing = parseEasing(inValue); }
            );
            applyList(
                TRANSITION_DELAY_ATTRIBUTE_NAME,
                [](StyleTransition& outTransition, const String& inValue) { outTransition.delay = parseTime(inValue); }
            );
        }

        void Style::parseAnimation(const StyleRuleset::Properties& inProperties)
        {
            const bool bHasShorthand = inProperties.find(ANIMATION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasName      = inProperties.find(ANIMATION_NAME_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasDuration  = inProperties.find(ANIMATION_DURATION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasEasing = inProperties.find(ANIMATION_TIMING_FUNCTION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasDelay  = inProperties.find(ANIMATION_DELAY_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasIterations =
                inProperties.find(ANIMATION_ITERATION_COUNT_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasDirection = inProperties.find(ANIMATION_DIRECTION_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasFill      = inProperties.find(ANIMATION_FILL_MODE_ATTRIBUTE_NAME) != inProperties.end();
            const bool bHasPlayState = inProperties.find(ANIMATION_PLAY_STATE_ATTRIBUTE_NAME) != inProperties.end();

            if (!bHasShorthand && !bHasName && !bHasDuration && !bHasEasing && !bHasDelay && !bHasIterations &&
                !bHasDirection && !bHasFill && !bHasPlayState)
            {
                return;
            }

            auto parseDirection = [&](const String& inValue)
            {
                const String value = inValue.trim().toLower();

                animation.bReverse =
                    value.equals(ANIMATION_DIRECTION_TYPE_REVERSE, ANIMATION_DIRECTION_TYPE_ALTERNATE_REVERSE);
                animation.bAlternate =
                    value.equals(ANIMATION_DIRECTION_TYPE_ALTERNATE, ANIMATION_DIRECTION_TYPE_ALTERNATE_REVERSE);
            };

            auto parseFill = [&](const String& inValue)
            {
                const String value = inValue.trim().toLower();

                animation.bFillForwards  = value.equals(ANIMATION_FILL_TYPE_FORWARDS, ANIMATION_FILL_TYPE_BOTH);
                animation.bFillBackwards = value.equals(ANIMATION_FILL_TYPE_BACKWARDS, ANIMATION_FILL_TYPE_BOTH);
            };

            auto parseIterations = [&](const String& inValue)
            {
                const String value = inValue.trim().toLower();

                if (value.equals(ANIMATION_ITERATION_INFINITE))
                {
                    animation.iterations = -1;

                    return;
                }

                if (value.isEmpty() || value.isNaN())
                {
                    animation.iterations = 1;

                    return;
                }

                animation.iterations = static_cast<int>(std::strtod(value.toChar(), nullptr));
            };

            if (bHasShorthand)
            {
                const std::vector<String> items = splitStyleList(inProperties.at(ANIMATION_ATTRIBUTE_NAME));

                if (!items.empty())
                {
                    bool bHasDurationToken = false;

                    for (const String& token : splitOneliner(items.at(0)))
                    {
                        if (isTime(token))
                        {
                            if (!bHasDurationToken)
                            {
                                animation.duration = parseTime(token);
                                bHasDurationToken  = true;

                                continue;
                            }

                            animation.delay = parseTime(token);

                            continue;
                        }

                        if (isEasing(token))
                        {
                            animation.easing = parseEasing(token);

                            continue;
                        }

                        const String value = token.trim().toLower();

                        if (value.equals(ANIMATION_ITERATION_INFINITE) || (!value.isEmpty() && !value.isNaN()))
                        {
                            parseIterations(value);

                            continue;
                        }

                        if (value.equals(
                                ANIMATION_DIRECTION_TYPE_NORMAL,
                                ANIMATION_DIRECTION_TYPE_REVERSE,
                                ANIMATION_DIRECTION_TYPE_ALTERNATE,
                                ANIMATION_DIRECTION_TYPE_ALTERNATE_REVERSE
                            ))
                        {
                            parseDirection(value);

                            continue;
                        }

                        if (value.equals(
                                ANIMATION_FILL_TYPE_NONE,
                                ANIMATION_FILL_TYPE_FORWARDS,
                                ANIMATION_FILL_TYPE_BACKWARDS,
                                ANIMATION_FILL_TYPE_BOTH
                            ))
                        {
                            parseFill(value);

                            continue;
                        }

                        if (value.equals(ANIMATION_PLAY_STATE_TYPE_RUNNING, ANIMATION_PLAY_STATE_TYPE_PAUSED))
                        {
                            animation.bPaused = value.equals(ANIMATION_PLAY_STATE_TYPE_PAUSED);

                            continue;
                        }

                        if (value.equals(ANIMATION_NAME_NONE))
                        {
                            animation.name = "";

                            continue;
                        }

                        animation.name = token.trim();
                    }
                }
            }

            if (bHasName)
            {
                const String name = inProperties.at(ANIMATION_NAME_ATTRIBUTE_NAME).trim();

                animation.name = name.equals(ANIMATION_NAME_NONE) ? "" : name;
            }

            if (bHasDuration)
            {
                animation.duration = parseTime(inProperties.at(ANIMATION_DURATION_ATTRIBUTE_NAME));
            }

            if (bHasEasing)
            {
                animation.easing = parseEasing(inProperties.at(ANIMATION_TIMING_FUNCTION_ATTRIBUTE_NAME));
            }

            if (bHasDelay)
            {
                animation.delay = parseTime(inProperties.at(ANIMATION_DELAY_ATTRIBUTE_NAME));
            }

            if (bHasIterations)
            {
                parseIterations(inProperties.at(ANIMATION_ITERATION_COUNT_ATTRIBUTE_NAME));
            }

            if (bHasDirection)
            {
                parseDirection(inProperties.at(ANIMATION_DIRECTION_ATTRIBUTE_NAME));
            }

            if (bHasFill)
            {
                parseFill(inProperties.at(ANIMATION_FILL_MODE_ATTRIBUTE_NAME));
            }

            if (bHasPlayState)
            {
                animation.bPaused = inProperties.at(ANIMATION_PLAY_STATE_ATTRIBUTE_NAME)
                                        .trim()
                                        .toLower()
                                        .equals(ANIMATION_PLAY_STATE_TYPE_PAUSED);
            }
        }

        std::vector<String> splitOneliner(const String& inValue)
        {
            std::vector<String> result;

            std::uint32_t start = 0;
            std::uint32_t end   = 0;

            std::uint32_t parathesisCount = 0;

            for (std::uint32_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);

                end++;

                if (character == METHOD_PARAMS_OPENING)
                {
                    parathesisCount++;

                    continue;
                }

                if (character == METHOD_PARAMS_CLOSING)
                {
                    parathesisCount--;

                    continue;
                }

                if (character != Style::ONELINE_SEPARATOR || parathesisCount > 0)
                {
                    continue;
                }

                const String block = inValue.substr(start, end - start);

                start = end;

                if (block.isEmpty())
                {
                    continue;
                }

                result.push_back(block.trim());
            }

            result.push_back(inValue.substr(start, end - start));

            return result;
        }
    }
}