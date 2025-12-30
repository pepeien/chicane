#include "Chicane/Grid/Style.hpp"

#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleSource::List Style::parseSources(const pugi::xml_node &inNode)
        {
            return parseSources(
                FileSystem::Path(
                    Xml::getAttribute(Style::ATTRIBUTE_NAME, inNode).as_string()
                )
            );
        }

        StyleSource::List Style::parseSources(const FileSystem::Path& inFilePath)
        {
            if (inFilePath.empty())
            {
                return {};
            }

            const String fileExtension = inFilePath.filename().extension().string();

            if (!fileExtension.endsWith(Style::FILE_EXTENSION_NAME))
            {
                return {};
            }

            return parseSources(FileSystem::readString(inFilePath));
        }

        StyleSource::List Style::parseSources(const String &inData)
        {
            String data = inData;
            data.erase(
                std::remove(
                    data.begin(),
                    data.end(),
                    '\n'
                ),
                data.cend()
            );
            data.erase(
                std::remove(
                    data.begin(),
                    data.end(),
                    '\r'
                ),
                data.cend()
            );

            const std::vector<String> styles = data.split('}');

            StyleSource::List result = {};

            for (const String &style : styles)
            {
                const std::vector<String> splittedStyle = style.split('{');

                if (splittedStyle.size() < 2)
                {
                    continue;
                }

                const String& selector = splittedStyle.at(0);
                result.emplace_back(
                    selector.split(Style::SELECTOR_SEPARATOR),
                    parseSource(splittedStyle.at(1))
                );
            }

            return result;
        }

        StyleProperties Style::parseSource(const String &inData)
        {
            std::vector<String> blocks = inData.split(';');

            StyleProperties result = {};

            for (const String &block : blocks)
            {
                const std::vector<String> splittedBlock = block.split(':');

                if (splittedBlock.size() < 2)
                {
                    continue;
                }

                const String key   = splittedBlock.at(0).trim();
                const String value = splittedBlock.at(1).trim();

                if (result.find(key) != result.end())
                {
                    result[key] = value;

                    continue;
                }

                result.insert(std::make_pair(key, value));
            }

            return result;
        }

        Style::Style(const StyleProperties& inProperties, Component* inParent)
            : Style()
        {
            setProperties(inProperties);
            setParent(inParent);
        }

        Style::Style()
            : display(StyleDisplay::Block),
            zIndex(0.0f),
            width(0.0f),
            height(0.0f),
            flex({}),
            position(StylePosition::Relative),
            align(StyleAlignment::Start),
            justify(StyleAlignment::Start),
            margin(
                Style::MARGIN_ATTRIBUTE_NAME,
                Style::MARGIN_TOP_ATTRIBUTE_NAME,
                Style::MARGIN_BOTTOM_ATTRIBUTE_NAME,
                Style::MARGIN_LEFT_ATTRIBUTE_NAME,
                Style::MARGIN_RIGHT_ATTRIBUTE_NAME
            ),
            padding(
                Style::PADDING_ATTRIBUTE_NAME,
                Style::PADDING_TOP_ATTRIBUTE_NAME,
                Style::PADDING_BOTTOM_ATTRIBUTE_NAME,
                Style::PADDING_LEFT_ATTRIBUTE_NAME,
                Style::PADDING_RIGHT_ATTRIBUTE_NAME
            ),
            gap(
                Style::GAP_ATTRIBUTE_NAME,
                Style::GAP_TOP_ATTRIBUTE_NAME,
                Style::GAP_BOTTOM_ATTRIBUTE_NAME,
                Style::GAP_LEFT_ATTRIBUTE_NAME,
                Style::GAP_RIGHT_ATTRIBUTE_NAME
            ),
            backgroundColor(Color::toRgba(Color::TEXT_COLOR_TRANSPARENT)),
            foregroundColor(Color::toRgba(Color::TEXT_COLOR_WHITE)),
            font(StyleFont()),
            m_properties({}),
            m_parent(nullptr)
        {}

        bool Style::isDisplay(StyleDisplay inValue) const
        {
            return display == inValue;
        }

        bool Style::isPosition(StylePosition inValue) const
        {
            return position == inValue;
        }

        bool Style::hasProperties() const
        {
            return !m_properties.empty();
        }

        void Style::setProperties(const StyleProperties& inProperties)
        {
            for (const auto& [name, value] : inProperties)
            {
                m_properties[name] = value;
            }

            refresh();
        }

        bool Style::hasParent() const
        {
            return m_parent != nullptr;
        }

        void Style::setParent(Component* inComponent)
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
            refreshForegroundColor();
            refreshBackgroundColor();
            refreshFont();
        }

        void Style::refreshDisplay()
        {
            if (m_properties.find(DISPLAY_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            const String value = parseText(m_properties.at(DISPLAY_ATTRIBUTE_NAME));

            if (value.equals(DISPLAY_TYPE_BLOCK))
            {
                setProperty(display, StyleDisplay::Block);
            }

            if (value.equals(DISPLAY_TYPE_FLEX))
            {
                setProperty(display, StyleDisplay::Flex);
            }

            if (value.equals(DISPLAY_TYPE_HIDDEN))
            {
                setProperty(display, StyleDisplay::Hidden);
            }
        }

        void Style::refreshFlex()
        {
            if (m_properties.find(FLEX_DIRECTION_ATTRIBUTE_NAME) != m_properties.end())
            {
                const String value = parseText(m_properties.at(FLEX_DIRECTION_ATTRIBUTE_NAME));

                if (value.equals(FLEX_DIRECTION_TYPE_COLUMN))
                {
                    setProperty(flex.direction, StyleFlex::Direction::Column);
                }

                if (value.equals(FLEX_DIRECTION_TYPE_ROW))
                {
                    setProperty(flex.direction, StyleFlex::Direction::Row);
                }
            }
        }

        void Style::refreshZIndex()
        {
            if (m_properties.find(Z_INDEX_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            setProperty(zIndex, parseNumber(m_properties.at(Z_INDEX_ATTRIBUTE_NAME)));
        }

        void Style::refreshSize()
        {
            if (m_properties.find(HEIGHT_ATTRIBUTE_NAME) != m_properties.end())
            {
                setProperty(
                    height,
                    parseSize(m_properties.at(HEIGHT_ATTRIBUTE_NAME), StyleDirection::Vertical)
                );
            }

            if (m_properties.find(WIDTH_ATTRIBUTE_NAME) != m_properties.end())
            {
                setProperty(
                    width,
                    parseSize(m_properties.at(WIDTH_ATTRIBUTE_NAME), StyleDirection::Horizontal)
                );
            }
        }

        void Style::refreshPosition()
        {
            if (m_properties.find(POSITION_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            const String value = parseText(m_properties.at(POSITION_ATTRIBUTE_NAME));

            if (value.startsWith(POSITION_TYPE_ABSOLUTE))
            {
                setProperty(position, StylePosition::Absolute);

                return;
            }

            setProperty(position, StylePosition::Relative);
        }

        void Style::refreshAlignment()
        {
            if (m_properties.find(ALIGNMENT_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            std::vector<String> values = {};

            for (const String& block : splitOneliner(m_properties.at(ALIGNMENT_ATTRIBUTE_NAME)))
            {
                values.push_back(parseText(block));
            }

            if (values.empty())
            {
                return;
            }

            setProperty(align, toAlignment(values.at(0)));
            setProperty(justify, values.size() == 1 ? align : toAlignment(values.at(1)));
        }

        void Style::refreshMargin()
        {
            if (
                !margin.refresh(
                    m_properties,
                    [this](const String& inValue, StyleDirection inDirection)
                    {
                        return parseSize(inValue, inDirection);
                    }
                )
            )
            {
                return;
            }

            emmitChanges();
        }

        void Style::refreshPadding()
        {
            if (
                !padding.refresh(
                    m_properties,
                    [this](const String& inValue, StyleDirection inDirection)
                    {
                        return parseSize(inValue, inDirection);
                    }
                )
            )
            {
                return;
            }

            emmitChanges();
        }

        void Style::refreshGap()
        {
            if (
                !gap.refresh(
                    m_properties,
                    [this](const String& inValue, StyleDirection inDirection)
                    {
                        return parseSize(inValue, inDirection);
                    }
                )
            )
            {
                return;
            }

            emmitChanges();
        }

        void Style::refreshForegroundColor()
        {
            if (m_properties.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            const Vec<4, std::uint32_t> color = parseColor(
                m_properties.at(FOREGROUND_COLOR_ATTRIBUTE_NAME)
            );

            if (Color::areEquals(foregroundColor, color))
            {
                return;
            }

            foregroundColor = color;

            emmitChanges();
        }

        void Style::refreshBackgroundColor()
        {
            if (m_properties.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == m_properties.end())
            {
                return;
            }

            const Vec<4, std::uint32_t> color = parseColor(
                m_properties.at(BACKGROUND_COLOR_ATTRIBUTE_NAME)
            );

            if (Color::areEquals(backgroundColor, color))
            {
                return;
            }

            backgroundColor = color;

            emmitChanges();
        }

        void Style::refreshFont()
        {
            if (m_properties.find(FONT_FAMILY_ATTRIBUTE_NAME) != m_properties.end())
            {
                setProperty(
                    font.family,
                    parseText(m_properties.at(FONT_FAMILY_ATTRIBUTE_NAME))
                );
            }

            if (m_properties.find(FONT_SIZE_ATTRIBUTE_NAME) != m_properties.end())
            {
                setProperty(
                    font.size,
                    parseSize(m_properties.at(FONT_SIZE_ATTRIBUTE_NAME), StyleDirection::Vertical)
                );
            }
        }

        Color::Rgba Style::parseColor(const String& inValue) const
        {
            String result = "";

            if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
            {
                const String keyword = inValue.startsWith(RGBA_KEYWORD) ?
                    RGBA_KEYWORD :
                    RGB_KEYWORD;

                const std::vector<String> params = inValue.getBetween(
                    FUNCTION_PARAMS_OPENING,
                    FUNCTION_PARAMS_CLOSING
                ).split(FUNCTION_PARAMS_SEPARATOR);

                if (params.empty())
                {
                    throw std::runtime_error("Invalid " + keyword + " parameters");
                }

                result.append(keyword);
                result.append(FUNCTION_PARAMS_OPENING);

                for (const String& param : params)
                {
                    result.append(parseText(param));
                    result.append(",");
                }

                if (result.endsWith(","))
                {
                    result.popBack();
                }

                result.append(FUNCTION_PARAMS_CLOSING);
            }
            else
            {
                result = parseText(inValue);
            }

            return Color::toRgba(result);
        }

        float Style::parseSize(const String& inValue, StyleDirection inDirection) const
        {
            const String value = parseText(inValue);

            if (value.startsWith(CALCULATION_KEYWORD))
            {
                return parseCalculation(value, inDirection);
            }

            if (value.equals(AUTO_SIZE_UNIT))
            {
                return parsePercentage("100%", inDirection);
            }

            if (value.endsWith(PERCENTAGE_SIZE_UNIT))
            {
                return parsePercentage(value, inDirection);
            }

            if (value.endsWith(VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return parseViewportHeight(value);
            }

            if (value.endsWith(VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return parseViewportWidth(value);
            }

            if (value.endsWith(PIXEL_SIZE_UNIT))
            {
                return parsePixel(value);
            }

            return parseNumber(value);
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
                value = variableToReference(value);
            }

            return m_parent->parseText(value);
        }

        float Style::parseCalculation(const String& inValue, StyleDirection inDirection) const
        {
            if (!inValue.startsWith(CALCULATION_KEYWORD))
            {
                return 0.0f;
            }

            const String operation = inValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING);

            std::uint32_t parathesisCount = 0;

            for (std::uint32_t i = 0; i < operation.size(); i++)
            {
                const char character = operation.at(i);

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

                const auto& iterator = std::find(
                    CALCULATION_OPERATORS.begin(),
                    CALCULATION_OPERATORS.end(),
                    character
                );

                if (iterator == CALCULATION_OPERATORS.end() || parathesisCount > 0)
                {
                    continue;
                }

                const float left  = parseSize(operation.substr(0, i), inDirection);
                const float right = parseSize(operation.substr(i + 1), inDirection);

                if (character == CALCULATION_OPERATOR_SUM)
                {
                    return left + right;
                }

                if (character == CALCULATION_OPERATOR_SUB)
                {
                    return left - right;
                }

                if (character == CALCULATION_OPERATOR_MUL)
                {
                    return left * right;
                }

                if (character == CALCULATION_OPERATOR_DIV)
                {
                    return left / right;
                }

                break;
            }

            return 0.0f;
        }

        float Style::parsePercentage(const String& inValue, StyleDirection inDirection) const
        {
            if (!inValue.endsWith(PERCENTAGE_SIZE_UNIT))
            {
                return 0.0f;
            }

            return parsePercentage(parseNumber(inValue, PERCENTAGE_SIZE_UNIT), inDirection);
        }

        float Style::parsePercentage(float inValue, StyleDirection inDirection) const
        {
            const float value = inValue / 100;

            if (
                !hasParent() ||
                !m_parent->hasRoot() || !m_parent->hasParent()
            )
            {
                return value;
            }

            const Vec2& size = isPosition(StylePosition::Absolute) ?
                m_parent->getRoot()->getSize() :
                m_parent->getParent()->getAvailableSize();

            if (inDirection == StyleDirection::Horizontal)
            {
                return std::max(0.0f, size.x) * value;
            }

            return std::max(0.0f, size.y) * value;
        }

        float Style::parseViewportHeight(const String& inValue) const
        {
            if (!inValue.endsWith(VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return 0.0f;
            }

            return parseViewportHeight(parseNumber(inValue, VIEWPORT_HEIGHT_SIZE_UNIT));
        }

        float Style::parseViewportHeight(float inValue) const
        {
            if (!hasParent())
            {
                return inValue;
            }

            return m_parent->getRoot()->getSize().y * (inValue / 100.0f);
        }

        float Style::parseViewportWidth(const String& inValue) const
        {
            if (!inValue.endsWith(VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return 0.0f;
            }

            return parseViewportWidth(parseNumber(inValue, VIEWPORT_WIDTH_SIZE_UNIT));
        }

        float Style::parseViewportWidth(float inValue) const
        {
            if (!hasParent())
            {
                return inValue;
            }

            return m_parent->getRoot()->getSize().x * (inValue / 100.0f);
        }

        float Style::parsePixel(const String& inValue) const
        {
            if (!inValue.endsWith(PIXEL_SIZE_UNIT))
            {
                return 0.0f;
            }

            return parseNumber(inValue, PIXEL_SIZE_UNIT);
        }

        float Style::parseNumber(const String& inValue, const String& inUnit) const
        {
            if (inValue.isEmpty() || inValue.size() < inUnit.size())
            {
                return 0.0f;
            }

            return parseNumber(inValue.substr(0, inValue.size() - inUnit.size()));
        }

        float Style::parseNumber(const String& inValue) const
        {
            try
            {
                const String value = inValue.trim();

                if (value.isNaN())
                {
                    return 0.0f;
                }

                char* end;
                return std::strtod(value.toChar(), &end);
            }
            catch(...)
            {
                return 0.0f;
            }
        }

        String variableToReference(const String& inValue)
        {
            const std::uint32_t start = inValue.firstOf(FUNCTION_PARAMS_OPENING) + 1;
            const std::uint32_t end   = inValue.lastOf(FUNCTION_PARAMS_CLOSING);

            String reference = "";
            reference.append(REFERENCE_VALUE_OPENING);
            reference.append(inValue.substr(start, end - start));
            reference.append(REFERENCE_VALUE_CLOSING);
            reference.append(inValue.substr(end + 1));

            return reference;
        }

        String colorToReference(const String& inValue)
        {
            if (inValue.startsWith(Style::VARIABLE_KEYWORD))
            {
                return variableToReference(inValue);
            }

            return inValue;
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