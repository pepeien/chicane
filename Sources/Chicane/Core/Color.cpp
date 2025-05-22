#include "Chicane/Core/Color.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Color
    {
        static std::unordered_map<std::string, Rgba> g_colors {
            { HEX_COLOR_TRANSPARENT,  Rgba(  0U,   0U,   0U,   0U) },
            { HEX_COLOR_RED,          Rgba(255U,   0U,   0U, 255U) },
            { HEX_COLOR_GREEN,        Rgba(  0U, 255U,   0U, 255U) },
            { HEX_COLOR_BLUE,         Rgba(  0U,   0U, 255U, 255U) },
            { HEX_COLOR_BLACK,        Rgba(  0U,   0U,   0U, 255U) },
            { HEX_COLOR_WHITE,        Rgba(255U, 255U, 255U, 255U) },

            { TEXT_COLOR_TRANSPARENT, Rgba(  0U,   0U,   0U,   0U) },
            { TEXT_COLOR_RED,         Rgba(255U,   0U,   0U, 255U) },
            { TEXT_COLOR_GREEN,       Rgba(  0U, 255U,   0U, 255U) },
            { TEXT_COLOR_BLUE,        Rgba(  0U,   0U, 255U, 255U) },
            { TEXT_COLOR_BLACK,       Rgba(  0U,   0U,   0U, 255U) },
            { TEXT_COLOR_WHITE,       Rgba(255U, 255U, 255U, 255U) }
        };

        bool areEquals(const Rgba& inA, const Rgba& inB)
        {
            return (
                inA.r == inB.r &&
                inA.g == inB.g &&
                inA.b == inB.b &&
                inA.a == inB.a
            );
        }

        bool isVisible(const std::string& inValue)
        {
            return !inValue.empty() &&
                   !String::areEquals(inValue, TEXT_COLOR_TRANSPARENT) &&
                   !String::areEquals(inValue, HEX_COLOR_TRANSPARENT);
        }

        bool isVisible(const Rgba& inValue)
        {
            return inValue.a > 0.0f;
        }

        Rgba toRgba(const std::string& inValue)
        {
            if (String::startsWith(inValue, HEX_KEYWORD))
            {
                std::string color = String::trim(inValue);
                std::transform(
                    color.begin(),
                    color.end(),
                    color.begin(),
                    ::tolower
                );
    
                bool bIsTransparent = color.empty() || String::areEquals(color, TEXT_COLOR_TRANSPARENT);
                bool bIsNotHex      = color.size() < 7 || color.size() > 9;
    
                if (bIsTransparent || bIsNotHex)
                {
                    color = HEX_COLOR_TRANSPARENT;
                }
    
                color = color.substr(1);
                color = color.size() == 6 ? color + "FF" : color;
    
                if (g_colors.find(color) == g_colors.end())
                {
                    std::uint16_t r = 0;
                    std::uint16_t g = 0;
                    std::uint16_t b = 0;
                    std::uint16_t a = 0;
    
                    sscanf(
                        color.c_str(),
                        "%02hx%02hx%02hx%02hx",
                        &r,
                        &g,
                        &b,
                        &a
                    );
    
                    const Rgba result = { r, g, b, a };
    
                    g_colors.insert(std::make_pair(color, result));
    
                    return result;
                }
    
                return g_colors.at(color);
            }

            if (
                String::startsWith(inValue, RGB_KEYWORD) ||
                String::startsWith(inValue, RGBA_KEYWORD)
            )
            {
                const std::uint32_t start = inValue.find_first_of("(") + 1;
                const std::uint32_t end   = inValue.find_last_of(")");

                const std::string color = inValue.substr(start, end - start);

                if (g_colors.find(color) == g_colors.end())
                {
                    const std::vector<std::string> values = String::split(color, ",");

                    for (const std::string& value : values)
                    {
                        if (!String::isNaN(value))
                        {
                            continue;
                        }

                        return g_colors.at(HEX_COLOR_TRANSPARENT);
                    }

                    Rgba result = Rgba(0U);
                    result.r = std::stoi(values.at(0));
                    result.g = std::stoi(values.at(1));
                    result.b = std::stoi(values.at(2));
                    result.a = values.size() < 4 ? 255 : std::stoi(values.at(3));
    
                    g_colors.insert(
                        std::make_pair(
                            color,
                            result
                        )
                    );

                    return result;
                }

                return g_colors.at(color);
            }

            return g_colors.at(HEX_COLOR_TRANSPARENT);
        }
    }
}