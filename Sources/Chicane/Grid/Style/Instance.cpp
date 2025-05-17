#include "Chicane/Grid/Style/Instance.hpp"

#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Sources Instance::parseSources(const pugi::xml_node &inNode)
            {
                return Instance::parseSources(
                    std::filesystem::path(
                        XML::getAttribute(ATTRIBUTE_NAME, inNode).as_string()
                    )
                );
            }

            Sources Instance::parseSources(const std::filesystem::path &inFilePath)
            {
                if (inFilePath.empty())
                {
                    return {};
                }

                std::string fileExtension = inFilePath.filename().extension().string();

                if (!String::endsWith(fileExtension, FILE_EXTENSION_NAME))
                {
                    return {};
                }

                std::vector<char> data = FileSystem::readFile(inFilePath);

                return Instance::parseSources(std::string(data.begin(), data.end()));
            }

            Sources Instance::parseSources(const std::string &inData)
            {
                std::string data = inData;
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

                std::vector<std::string> styles = String::split(
                    data,
                    '}'
                );

                Sources result = {};

                for (const std::string &style : styles)
                {
                    std::vector<std::string> splittedStyle = String::split(style, '{');

                    if (splittedStyle.size() < 2)
                    {
                        continue;
                    }

                    Source source = {};
                    source.selectors  = String::split(splittedStyle.at(0), SELECTOR_SEPARATOR);
                    source.properties = Instance::parseSource(splittedStyle.at(1));

                    result.push_back(source);
                }

                return result;
            }

            Properties Instance::parseSource(const std::string &inData)
            {
                std::vector<std::string> blocks = String::split(
                    inData,
                    ';'
                );

                Properties result = {};

                for (const std::string &block : blocks)
                {
                    std::vector<std::string> splittedBlock = String::split(
                        block,
                        ':'
                    );

                    if (splittedBlock.size() < 2)
                    {
                        continue;
                    }

                    std::string key   = String::trim(splittedBlock.at(0));
                    std::string value = String::trim(splittedBlock.at(1));

                    if (result.find(key) != result.end())
                    {
                        result[key] = value;

                        continue;
                    }

                    result.insert(
                        std::make_pair(
                            key,
                            value
                        )
                    );
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
                font({}),
                m_properties({}),
                m_parent(nullptr),
                m_changesObersable(std::make_unique<Observable<void*>>())
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
                m_properties = inProperties;
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

            void Instance::emmitChanges()
            {
                m_changesObersable->next(nullptr);
            }

            Subscription<void*>* Instance::watchChanges(
                std::function<void (void*)> inNext,
                std::function<void (const std::string&)> inError,
                std::function<void ()> inComplete
            ) const
            {
                return m_changesObersable->subscribe(inNext, inError, inComplete);
            }

            void Instance::refreshDisplay()
            {
                if (m_properties.find(DISPLAY_ATTRIBUTE_NAME) == m_properties.end())
                {
                    return;
                }

                const std::string value = parseText(m_properties.at(DISPLAY_ATTRIBUTE_NAME));

                if (String::areEquals(value, DISPLAY_TYPE_BLOCK))
                {
                    setProperty(display, Display::Block);
                }

                if (String::areEquals(value, DISPLAY_TYPE_FLEX))
                {
                    setProperty(display, Display::Flex);
                }

                if (String::areEquals(value, DISPLAY_TYPE_HIDDEN))
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

                const std::string value = parseText(m_properties.at(DISPLAY_ATTRIBUTE_NAME));

                if (String::areEquals(value, POSITION_TYPE_ABSOLUTE))
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

                std::vector<std::string> values = {};

                for (const std::string& block : splitOneliner(m_properties.at(ALIGNMENT_ATTRIBUTE_NAME)))
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
                        [&](const std::string& inValue, Direction inDirection)
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

                setProperty(
                    foregroundColor,
                    parseColor(m_properties.at(FOREGROUND_COLOR_ATTRIBUTE_NAME))
                );
            }

            void Instance::refreshBackgroundColor()
            {
                if (m_properties.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == m_properties.end())
                {
                    return;
                }

                setProperty(
                    backgroundColor,
                    parseColor(m_properties.at(BACKGROUND_COLOR_ATTRIBUTE_NAME))
                );
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

            Color::Rgba Instance::parseColor(const std::string& inValue) const
            {
                std::string result = "";

                if (
                    String::startsWith(inValue, RGB_KEYWORD) ||
                    String::startsWith(inValue, RGBA_KEYWORD)
                )
                {
                    const std::string keyword = String::startsWith(inValue, RGBA_KEYWORD) ?
                        RGBA_KEYWORD :
                        RGB_KEYWORD;
    
                    std::string rawParams = extractParams(inValue);
    
                    if (rawParams.empty())
                    {
                        throw std::runtime_error("Invalid " + keyword + " parameters");
                    }
    
                    std::vector<std::string> splittedParams = String::split(
                        rawParams,
                        FUNCTION_PARAMS_SEPARATOR
                    );
    
                    result.append(keyword);
                    result.push_back(FUNCTION_PARAMS_OPENING);
    
                    for (const std::string& param : splittedParams)
                    {
                        result.append(parseText(colorToReference(param)));
                        result.append(",");
                    }
    
                    if (String::endsWith(result, ","))
                    {
                        result.pop_back();
                    }
    
                    result.push_back(FUNCTION_PARAMS_CLOSING);
                }
                else
                {
                    result = parseText(inValue);
                }
    
                return Color::toRgba(result);
            }

            float Instance::parseSize(const std::string& inValue, Direction inDirection) const
            {
                std::string value = parseText(inValue);

                if (String::startsWith(inValue, CALCULATION_KEYWORD))
                {
                    return parseCalculation(inValue, inDirection);
                }

                if (String::areEquals(inValue, AUTO_SIZE_UNIT))
                {
                    return parsePercentage("100%", inDirection);
                }

                if (String::endsWith(inValue, PERCENTAGE_SIZE_UNIT))
                {
                    return parsePercentage(inValue, inDirection);
                }

                if (String::endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
                {
                    return parseViewportHeight(inValue);
                }

                if (String::endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
                {
                    return parseViewportWidth(inValue);
                }

                if (String::endsWith(inValue, PIXEL_SIZE_UNIT))
                {
                    return parsePixel(inValue);
                }

                if (!String::isNaN(inValue))
                {
                    return std::stof(inValue);
                }

                return 0.0f;
            }

            std::string Instance::parseText(const std::string& inValue) const
            {
                std::string result = inValue;

                if (String::startsWith(result, VARIABLE_KEYWORD))
                {
                    result = variableToReference(result);
                }

                return m_parent->parseText(result);
            }

            float Instance::parseCalculation(const std::string& inValue, Direction inDirection) const
            {
                if (!String::startsWith(inValue, CALCULATION_KEYWORD))
                {
                    return 0.0f;
                }
    
                const std::string operation = extractParams(inValue);
    
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
    
                    const float left  = parseSize(String::trim(operation.substr(0, i)), inDirection);
                    const float right = parseSize(String::trim(operation.substr(i + 1)), inDirection);
    
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
    
            float Instance::parsePercentage(const std::string& inValue, Direction inDirection) const
            {
                if (!String::endsWith(inValue, PERCENTAGE_SIZE_UNIT))
                {
                    return 0.0f;
                }
    
                return parsePercentage(
                    std::stof(
                        inValue.substr(
                            0,
                            inValue.length() - strlen(PERCENTAGE_SIZE_UNIT)
                        )
                    ),
                    inDirection
                );
            }
    
            float Instance::parsePercentage(float inValue, Direction inDirection) const
            {
                float value = inValue / 100;
    
                if (!m_parent->hasRoot() || !m_parent->hasParent())
                {
                    return value;
                }
    
                const Vec<2, float>& size = isPosition(Position::Absolute) ?
                    m_parent->getRoot()->getSize() :
                    m_parent->getSize();
    
                const Vec<2, float>& offset = isPosition(Position::Absolute) ?
                    Vec2Zero :
                    m_parent->getCursor();
    
                if (inDirection == Direction::Horizontal)
                {
                    return std::max(0.0f, size.x - offset.x) * value;
                }
    
                return std::max(0.0f, size.y - offset.y) * value;
            }
    
            float Instance::parseViewportHeight(const pugi::xml_attribute& inAttribute) const
            {
                if (inAttribute.empty())
                {
                    return 0.0f;
                }
    
                return parseViewportHeight(inAttribute.as_string());
            }
    
            float Instance::parseViewportHeight(const std::string& inValue) const
            {
                if (!String::endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
                {
                    return 0.0f;
                }
    
                return parseViewportHeight(
                    std::stof(
                        inValue.substr(
                            0,
                            inValue.length() - strlen(VIEWPORT_HEIGHT_SIZE_UNIT)
                        )
                    )
                );
            }
    
            float Instance::parseViewportHeight(float inVhValue) const
            {
                return m_parent->getRoot()->getSize().y * (inVhValue / 100.0f);
            }
    
            float Instance::parseViewportWidth(const pugi::xml_attribute& inAttribute) const
            {
                if (inAttribute.empty())
                {
                    return 0.0f;
                }
    
                return parseViewportWidth(inAttribute.as_string());
            }
    
            float Instance::parseViewportWidth(const std::string& inValue) const
            {
                if (!String::endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
                {
                    return 0.0f;
                }
    
                return parseViewportHeight(
                    std::stof(
                        inValue.substr(
                            0,
                            inValue.length() - strlen(VIEWPORT_WIDTH_SIZE_UNIT)
                        )
                    )
                );
            }
    
            float Instance::parseViewportWidth(float inVwValue) const
            {
                return m_parent->getRoot()->getSize().x * (inVwValue / 100.0f);
            }
    
            float Instance::parsePixel(const pugi::xml_attribute& inAttribute) const
            {
                if (inAttribute.empty())
                {
                    return 0.0f;
                }
    
                return parsePixel(inAttribute.as_string());
            }
    
            float Instance::parsePixel(const std::string& inValue) const
            {
                if (!String::endsWith(inValue, PIXEL_SIZE_UNIT))
                {
                    return 0.0f;
                }
            
                return std::stof(
                    inValue.substr(
                        0,
                        inValue.length() - strlen(PIXEL_SIZE_UNIT)
                    )
                );
            }
        }
    }
}