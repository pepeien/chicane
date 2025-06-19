#include "Chicane/Core/Color.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Color
    {
        static std::unordered_map<String, Rgba> g_colors {
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

        bool isVisible(const String& inValue)
        {
            return !inValue.isEmpty() &&
                   !inValue.equals(TEXT_COLOR_TRANSPARENT) &&
                   !inValue.equals(HEX_COLOR_TRANSPARENT);
        }

        bool isVisible(const Rgba& inValue)
        {
            return inValue.a > 0.0f;
        }

        Rgba toRgba(const String& inValue)
        {
            if (inValue.startsWith(HEX_KEYWORD))
            {
                String color = inValue.trim().toUpper();
    
                const bool bIsTransparent = color.equals(HEX_COLOR_TRANSPARENT);
                const bool bIsNotHex      = color.size() < 7 || color.size() > 9;
    
                if (bIsTransparent || bIsNotHex)
                {
                    color = HEX_COLOR_TRANSPARENT;
                }
    
                color = color.substr(1);
                color = color.size() == 6 ? color + "FF" : color;
    
                if (g_colors.find(color) == g_colors.end())
                {
                    Rgba result = Rgba(0U);
                    sscanf(
                        color.toChar(),
                        "%02hhx%02hhx%02hhx%02hhx",
                        &result.r,
                        &result.g,
                        &result.b,
                        &result.a
                    );
                    g_colors.insert(std::make_pair(color, result));
    
                    return result;
                }
    
                return g_colors.at(color);
            }

            if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
            {
                const std::uint32_t start = inValue.firstOf("(") + 1;
                const std::uint32_t end   = inValue.lastOf(")");

                const String color = inValue.substr(start, end - start);

                if (g_colors.find(color) == g_colors.end())
                {
                    const std::vector<String> values = color.split(",");

                    for (const String& value : values)
                    {
                        if (!color.isNaN())
                        {
                            continue;
                        }

                        return g_colors.at(HEX_COLOR_TRANSPARENT);
                    }

                    Rgba result = Rgba(
                        std::stoi(values.at(0).toStandard()),
                        std::stoi(values.at(1).toStandard()),
                        std::stoi(values.at(2).toStandard()),
                        values.size() < 4 ? 255U : std::stoi(values.at(3).toStandard())
                    );
                    g_colors.insert(std::make_pair(color, result));

                    return result;
                }

                return g_colors.at(color);
            }

            return g_colors.at(HEX_COLOR_TRANSPARENT);
        }
    }
}