#include "Chicane/Grid/Style/Instance.hpp"

#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Source::List Instance::parseSources(const pugi::xml_node &inNode)
            {
                return Instance::parseSources(
                    FileSystem::Path(
                        Xml::getAttribute(ATTRIBUTE_NAME, inNode).as_string()
                    )
                );
            }

            Source::List Instance::parseSources(const FileSystem::Path &inFilePath)
            {
                if (inFilePath.empty())
                {
                    return {};
                }

                const String fileExtension = inFilePath.filename().extension().string();

                if (!fileExtension.endsWith(FILE_EXTENSION_NAME))
                {
                    return {};
                }

                return Instance::parseSources(FileSystem::readString(inFilePath));
            }

            Source::List Instance::parseSources(const String &inData)
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

                Source::List result = {};

                for (const String &style : styles)
                {
                    const std::vector<String> splittedStyle = style.split('{');

                    if (splittedStyle.size() < 2)
                    {
                        continue;
                    }

                    const String& selector = splittedStyle.at(0);
                    result.emplace_back(
                        selector.split(SELECTOR_SEPARATOR),
                        Instance::parseSource(splittedStyle.at(1))
                    );
                }

                return result;
            }

            Properties Instance::parseSource(const String &inData)
            {
                std::vector<String> blocks = inData.split(';');

                Properties result = {};

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

            Instance::Instance(const Properties& inProperties, Component* inParent)
                : Instance()
            {
                setProperties(inProperties);
                setParent(inParent);
            }

            Instance::Instance()
                : display(Display::Block),
                width(0.0f),
                height(0.0f),
                flex({}),
                position(Position::Relative),
                align(Alignment::Start),
                justify(Alignment::Start),
                margin(
                    MARGIN_ATTRIBUTE_NAME,
                    MARGIN_TOP_ATTRIBUTE_NAME,
                    MARGIN_BOTTOM_ATTRIBUTE_NAME,
                    MARGIN_LEFT_ATTRIBUTE_NAME,
                    MARGIN_RIGHT_ATTRIBUTE_NAME
                ),
                backgroundColor(Color::toRgba(Color::TEXT_COLOR_TRANSPARENT)),
                foregroundColor(Color::toRgba(Color::TEXT_COLOR_WHITE)),
                font(Font()),
                m_properties({}),
                m_parent(nullptr)
            {}

            bool Instance::isDisplay(Display inValue) const
            {
                return display == inValue;
            }

            bool Instance::isPosition(Position inValue) const
            {
                return position == inValue;
            }

            bool Instance::hasProperties() const
            {
                return !m_properties.empty();
            }

            void Instance::setProperties(const Properties& inProperties)
            {
                for (const auto& [name, value] : inProperties)
                {
                    m_properties[name] = value;
                }

                refresh();
            }

            bool Instance::hasParent() const
            {
                return m_parent != nullptr;
            }

            void Instance::setParent(Component* inComponent)
            {
                if (m_parent == inComponent)
                {
                    return;
                }

                m_parent = inComponent;

                refresh();
            }

            void Instance::refresh()
            {
                refreshDisplay();
                refreshSize();
                refreshPosition();
                refreshMargin();
                refreshAlignment();
                refreshForegroundColor();
                refreshBackgroundColor();
                refreshFont();
            }

            void Instance::refreshDisplay()
            {
                if (m_properties.find(DISPLAY_ATTRIBUTE_NAME) == m_properties.end())
                {
                    return;
                }

                const String value = parseText(m_properties.at(DISPLAY_ATTRIBUTE_NAME));

                if (value.equals(DISPLAY_TYPE_BLOCK))
                {
                    setProperty(display, Display::Block);
                }

                if (value.equals(DISPLAY_TYPE_FLEX))
                {
                    setProperty(display, Display::Flex);
                }

                if (value.equals(DISPLAY_TYPE_HIDDEN))
                {
                    setProperty(display, Display::Hidden);
                }
            }

            void Instance::refreshSize()
            {
                if (m_properties.find(HEIGHT_ATTRIBUTE_NAME) != m_properties.end())
                {
                    setProperty(
                        height,
                        parseSize(m_properties.at(HEIGHT_ATTRIBUTE_NAME), Direction::Vertical)
                    );
                }

                if (m_properties.find(WIDTH_ATTRIBUTE_NAME) != m_properties.end())
                {
                    setProperty(
                        width,
                        parseSize(m_properties.at(WIDTH_ATTRIBUTE_NAME), Direction::Horizontal)
                    );
                }
            }

            void Instance::refreshPosition()
            {
                if (m_properties.find(POSITION_ATTRIBUTE_NAME) == m_properties.end())
                {
                    return;
                }

                const String value = parseText(m_properties.at(POSITION_ATTRIBUTE_NAME));

                if (value.startsWith(POSITION_TYPE_ABSOLUTE))
                {
                    setProperty(position, Position::Absolute);

                    return;
                }

                setProperty(position, Position::Relative);
            }

            void Instance::refreshAlignment()
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

            void Instance::refreshMargin()
            {
                if (
                    !margin.refresh(
                        m_properties,
                        [this](const String& inValue, Direction inDirection)
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

            void Instance::refreshForegroundColor()
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

            void Instance::refreshBackgroundColor()
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

            void Instance::refreshFont()
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
                        parseSize(m_properties.at(FONT_SIZE_ATTRIBUTE_NAME), Direction::Vertical)
                    );
                }
            }

            Color::Rgba Instance::parseColor(const String& inValue) const
            {
                String result = "";

                if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
                {
                    const String keyword = inValue.startsWith(RGBA_KEYWORD) ?
                        RGBA_KEYWORD :
                        RGB_KEYWORD;

                    String rawParams = extractParams(inValue);

                    if (rawParams.isEmpty())
                    {
                        throw std::runtime_error("Invalid " + keyword + " parameters");
                    }

                    const std::vector<String> params = rawParams.split(FUNCTION_PARAMS_SEPARATOR);

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

            float Instance::parseSize(const String& inValue, Direction inDirection) const
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

            String Instance::parseText(const String& inValue) const
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

            float Instance::parseCalculation(const String& inValue, Direction inDirection) const
            {
                if (!inValue.startsWith(CALCULATION_KEYWORD))
                {
                    return 0.0f;
                }

                const String operation = extractParams(inValue);

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

            float Instance::parsePercentage(const String& inValue, Direction inDirection) const
            {
                if (!inValue.endsWith(PERCENTAGE_SIZE_UNIT))
                {
                    return 0.0f;
                }

                return parsePercentage(parseNumber(inValue, PERCENTAGE_SIZE_UNIT), inDirection);
            }

            float Instance::parsePercentage(float inValue, Direction inDirection) const
            {
                const float value = inValue / 100;

                if (
                    !hasParent() ||
                    !m_parent->hasRoot() || !m_parent->hasParent()
                )
                {
                    return value;
                }

                const Vec2& size = isPosition(Position::Absolute) ?
                    m_parent->getRoot()->getSize() :
                    m_parent->getParent()->getAvailableSize();

                if (inDirection == Direction::Horizontal)
                {
                    return std::max(0.0f, size.x) * value;
                }

                return std::max(0.0f, size.y) * value;
            }

            float Instance::parseViewportHeight(const String& inValue) const
            {
                if (!inValue.endsWith(VIEWPORT_HEIGHT_SIZE_UNIT))
                {
                    return 0.0f;
                }

                return parseViewportHeight(parseNumber(inValue, VIEWPORT_HEIGHT_SIZE_UNIT));
            }

            float Instance::parseViewportHeight(float inValue) const
            {
                if (!hasParent())
                {
                    return inValue;
                }

                return m_parent->getRoot()->getSize().y * (inValue / 100.0f);
            }

            float Instance::parseViewportWidth(const String& inValue) const
            {
                if (!inValue.endsWith(VIEWPORT_WIDTH_SIZE_UNIT))
                {
                    return 0.0f;
                }
    
                return parseViewportWidth(parseNumber(inValue, VIEWPORT_WIDTH_SIZE_UNIT));
            }

            float Instance::parseViewportWidth(float inValue) const
            {
                if (!hasParent())
                {
                    return inValue;
                }

                return m_parent->getRoot()->getSize().x * (inValue / 100.0f);
            }

            float Instance::parsePixel(const String& inValue) const
            {
                if (!inValue.endsWith(PIXEL_SIZE_UNIT))
                {
                    return 0.0f;
                }
            
                return parseNumber(inValue, PIXEL_SIZE_UNIT);
            }

            float Instance::parseNumber(const String& inValue, const String& inUnit) const
            {
                if (inValue.isEmpty() || inValue.size() < inUnit.size())
                {
                    return 0.0f;
                }

                return parseNumber(inValue.substr(0, inValue.size() - inUnit.size()));
            }

            float Instance::parseNumber(const String& inValue) const
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
        }
    }
}