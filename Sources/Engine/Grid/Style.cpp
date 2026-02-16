#include "Chicane/Grid/Style.hpp"

#include <algorithm>

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
              gap({}),
              background({}),
              foregroundColor(Color::toRgba(Color::TEXT_COLOR_WHITE)),
              opacity(OPACITY_DEFAULT_VALUE),
              font({}),
              letterSpacing(0.0f),
              m_parent(nullptr)
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
                    std::vector<String> values = {};

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

            gap.parseWith([this](const String& inValue, SizeDirection inDirection)
                          { return parseSize(inValue, inDirection); });

            background.parseWith(
                [this](const String& inValue) { return parseColor(inValue); },
                [this](const String& inValue) { return parseText(inValue); }
            );

            foregroundColor.parseWith([this](const String& inValue) { return parseColor(inValue); });

            opacity.parseWith([this](const String& inValue) { return parseSize(inValue, SizeDirection::Horizontal); });

            font.parseWith(
                [this](const String& inValue) { return parseText(inValue); },
                [this](const String& inValue) { return parseSize(inValue, SizeDirection::Vertical); }
            );

            letterSpacing.parseWith([this](const String& inValue)
                                    { return parseSize(inValue, SizeDirection::Horizontal); });
        }

        bool Style::isDisplay(StyleDisplay inValue) const
        {
            return display.get() == inValue;
        }

        bool Style::isPosition(StylePosition inValue) const
        {
            return position.get() == inValue;
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

            if (inProperties.find(HEIGHT_ATTRIBUTE_NAME) != inProperties.end())
            {
                height.setRaw(inProperties.at(HEIGHT_ATTRIBUTE_NAME));
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

            if (inProperties.find(LETTER_SPACING_ATTRIBUTE_NAME) != inProperties.end())
            {
                letterSpacing.setRaw(inProperties.at(LETTER_SPACING_ATTRIBUTE_NAME));
            }

            flex.setProperties(inProperties);

            background.setProperties(inProperties);

            font.setProperties(inProperties);

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

            gap.setProperties(
                inProperties,
                Style::GAP_ATTRIBUTE_NAME,
                Style::GAP_TOP_ATTRIBUTE_NAME,
                Style::GAP_BOTTOM_ATTRIBUTE_NAME,
                Style::GAP_LEFT_ATTRIBUTE_NAME,
                Style::GAP_RIGHT_ATTRIBUTE_NAME
            );

            refresh();
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
            refreshGap();
            refreshAlignment();
            refreshBackground();
            refreshForegroundColor();
            refreshOpacity();
            refreshFont();
            refreshLetterSpacing();
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

        void Style::refreshGap()
        {
            gap.refresh();
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

        void Style::refreshFont()
        {
            font.refresh();
        }

        void Style::refreshLetterSpacing()
        {
            letterSpacing.refresh();
        }

        Color::Rgba Style::parseColor(const String& inValue) const
        {
            String result = "";

            if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
            {
                const String keyword = inValue.startsWith(RGBA_KEYWORD) ? RGBA_KEYWORD : RGB_KEYWORD;

                const std::vector<String> params = inValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING)
                                                       .split(FUNCTION_PARAMS_SEPARATOR);

                if (params.empty())
                {
                    throw std::runtime_error("Invalid " + keyword + " parameters");
                }

                result.append(keyword);
                result.append(FUNCTION_PARAMS_OPENING);
                for (const String& param : params)
                {
                    result.append(parseText(param.trim()));
                    result.append(",");
                }
                result.popBack();
                result.append(FUNCTION_PARAMS_CLOSING);
            }
            else
            {
                result = parseText(inValue);
            }

            return Color::toRgba(result);
        }

        float Style::parseSize(const String& inValue, SizeDirection inDirection) const
        {
            Size result;
            result.setIsAsobute(isPosition(StylePosition::Absolute));
            result.setFontSize(Box::Font::BASE_SIZE);
            result.setTextParser([this](const String& inValue) { return parseText(inValue); });

            if (hasParent() && m_parent->hasParent())
            {
                result.setRoot(m_parent->getRoot()->getSize());
                result.setParent(m_parent->getParent()->getAvailableSize());
            }

            return result.parse(inValue, inDirection);
        }

        String Style::parseReference(const String& inValue) const
        {
            const std::uint32_t start = inValue.firstOf(FUNCTION_PARAMS_OPENING) + 1;
            const std::uint32_t end   = inValue.lastOf(FUNCTION_PARAMS_CLOSING);

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

            if (value.startsWith(VARIABLE_KEYWORD) && m_parent->hasStyleFile())
            {
                value = parseText(m_parent->getStyleFile()->getVariable(value.substr(1)));
            }

            if (value.startsWith(REFERENCE_KEYWORD))
            {
                value = parseReference(value);
            }

            return m_parent->parseText(value);
        }

        std::vector<String> splitOneliner(const String& inValue)
        {
            std::vector<String> result = {};

            std::uint32_t start = 0;
            std::uint32_t end   = 0;

            std::uint32_t parathesisCount = 0;

            for (std::uint32_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);

                end++;

                if (character == FUNCTION_PARAMS_OPENING)
                {
                    parathesisCount++;

                    continue;
                }

                if (character == FUNCTION_PARAMS_CLOSING)
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