#include "Grid/Style/Essential.hpp"

#include "Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            std::unordered_map<std::string, Vec<4, std::uint32_t>> m_colors {
                { HEX_COLOR_TRANSPARENT, Vec<4, std::uint32_t>(0) }
            };
    
            Vec<4, std::uint32_t> hexToRgba(const std::string& inValue)
            {
                std::string color = String::trim(inValue);
                std::transform(
                    color.begin(),
                    color.end(),
                    color.begin(),
                    ::tolower
                );
    
                bool bIsTransparent = color.empty() ||
                                      String::areEquals(color, BACKGROUND_COLOR_TRANSPARENT);
                bool bIsNotHex      = color.size() < 7 || color.size() > 9;
    
                if (bIsTransparent || bIsNotHex)
                {
                    color = HEX_COLOR_TRANSPARENT;
                }
    
                color = color.substr(1);
                color = color.size() == 6 ? color + "FF" : color;
    
                if (m_colors.find(color) == m_colors.end())
                {
                    Vec<4, std::uint32_t> result = Vec<4, std::uint32_t>(0U);
                    
                    sscanf(
                        color.c_str(),
                        "%02hx%02x%02hx%02hx",
                        &result.r,
                        &result.g,
                        &result.b,
                        &result.a
                    );
    
                    m_colors.insert(
                        std::make_pair(
                            color,
                            result
                        )
                    );
    
                    return result;
                }
    
                return m_colors.at(color);
            }
    
            Vec<4, std::uint32_t> toRgba(const std::string& inValue)
            {
                if (String::startsWith(inValue, HEX_KEYWORD))
                {
                    return hexToRgba(inValue);
                }
    
                if (
                    String::startsWith(inValue, RGB_KEYWORD) ||
                    String::startsWith(inValue, RGBA_KEYWORD)
                )
                {
                    const std::string color = extractParams(inValue);

                    if (m_colors.find(color) == m_colors.end())
                    {
                        const std::vector<std::string> values = String::split(color, ',');
    
                        Vec<4, std::uint32_t> result = Vec<4, std::uint32_t>(0U);
                        result.r = std::stoi(values.at(0));
                        result.g = std::stoi(values.at(1));
                        result.b = std::stoi(values.at(2));
                        result.a = values.size() < 3 ? 255 : std::stoi(values.at(3));
        
                        m_colors.insert(
                            std::make_pair(
                                color,
                                result
                            )
                        );
    
                        return result;
                    }
    
                    return m_colors.at(color);
                }
    
                return m_colors.at(HEX_COLOR_TRANSPARENT);
            }

            std::string variableToReference(const std::string& inValue)
            {
                const std::uint32_t start = inValue.find_first_of(FUNCTION_PARAMS_OPENING) + 1;
                const std::uint32_t end   = inValue.find_last_of(FUNCTION_PARAMS_CLOSING);

                std::string reference = "";
                reference.append(REFERENCE_VALUE_OPENING);
                reference.append(inValue.substr(start, end - start));
                reference.append(REFERENCE_VALUE_CLOSING);
                reference.append(inValue.substr(end + 1));

                return reference;
            }

            std::string colorToReference(const std::string& inValue)
            {
                if (String::startsWith(inValue, VARIABLE_KEYWORD))
                {
                    return variableToReference(inValue);
                }

                return inValue;
            }
        }
    }
}