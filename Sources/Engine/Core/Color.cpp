#include "Chicane/Core/Color.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Color
    {
        static std::unordered_map<String, Rgba> g_colors = {
            {HEX_COLOR_TRANSPARENT,  Rgba(0U,   0U,   0U,   0U)  },
            {HEX_COLOR_RED,          Rgba(255U, 0U,   0U,   255U)},
            {HEX_COLOR_GREEN,        Rgba(0U,   255U, 0U,   255U)},
            {HEX_COLOR_BLUE,         Rgba(0U,   0U,   255U, 255U)},
            {HEX_COLOR_BLACK,        Rgba(0U,   0U,   0U,   255U)},
            {HEX_COLOR_WHITE,        Rgba(255U, 255U, 255U, 255U)},

            {TEXT_COLOR_TRANSPARENT, Rgba(0U,   0U,   0U,   0U)  },
            {TEXT_COLOR_RED,         Rgba(255U, 0U,   0U,   255U)},
            {TEXT_COLOR_GREEN,       Rgba(0U,   255U, 0U,   255U)},
            {TEXT_COLOR_BLUE,        Rgba(0U,   0U,   255U, 255U)},
            {TEXT_COLOR_BLACK,       Rgba(0U,   0U,   0U,   255U)},
            {TEXT_COLOR_WHITE,       Rgba(255U, 255U, 255U, 255U)}
        };

        bool areEquals(const Rgba& inA, const Rgba& inB)
        {
            return (inA.r == inB.r && inA.g == inB.g && inA.b == inB.b && inA.a == inB.a);
        }

        bool isVisible(const String& inValue)
        {
            return !inValue.isEmpty() && !inValue.equals(TEXT_COLOR_TRANSPARENT) &&
                   !inValue.equals(HEX_COLOR_TRANSPARENT);
        }

        bool isVisible(const Rgba& inValue)
        {
            return inValue.a > 0.0f;
        }

        String normalizeColor(const String& inValue)
        {
            String result;
            String current;
            int    depth = 0;

            auto flush = [&](String token)
            {
                token = token.trim();

                if (token.isEmpty())
                {
                    return;
                }

                if (token.startsWith(RGB_KEYWORD))
                {
                    std::uint32_t start = token.firstOf('(') + 1;
                    std::uint32_t end   = token.lastOf(')');

                    result.append(normalizeColor(token.substr(start, end - start)));
                }
                else
                {
                    result.append(token);
                }

                result.append(',');
            };

            for (char character : inValue)
            {
                if (character == '(')
                {
                    depth++;
                }
                else if (character == ')')
                {
                    depth--;
                }

                if (character == ',' && depth == 0)
                {
                    flush(current);

                    current = "";
                }
                else
                {
                    current += character;
                }
            }

            flush(current);

            result.popBack();

            return result;
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
                    sscanf(color.toChar(), "%02hhx%02hhx%02hhx%02hhx", &result.r, &result.g, &result.b, &result.a);
                    g_colors.insert(std::make_pair(color, result));

                    return result;
                }

                return g_colors.at(color);
            }

            if (inValue.startsWith(RGB_KEYWORD) || inValue.startsWith(RGBA_KEYWORD))
            {
                const std::uint32_t start = inValue.firstOf("(") + 1;
                const std::uint32_t end   = inValue.lastOf(")");

                const String color = normalizeColor(inValue.substr(start, end - start));

                if (g_colors.find(color) != g_colors.end())
                {
                    return g_colors.at(color);
                }

                std::vector<String> values = color.split(',');

                if (values.size() < 3)
                {
                    return g_colors.at(HEX_COLOR_TRANSPARENT);
                }

                for (const String& value : values)
                {
                    if (!value.isNaN())
                    {
                        continue;
                    }

                    return g_colors.at(HEX_COLOR_TRANSPARENT);
                }

                Rgba result = Rgba(
                    std::stoi(values.at(0).toStandard()),
                    std::stoi(values.at(1).toStandard()),
                    std::stoi(values.at(2).toStandard()),
                    255U * (values.size() < 4 ? 1.0f : std::stof(values.at(3).toStandard()))
                );

                g_colors.insert(std::make_pair(color, result));

                return result;
            }

            if (g_colors.find(inValue) == g_colors.end())
            {
                return g_colors.at(HEX_COLOR_TRANSPARENT);
            }

            return g_colors.at(inValue);
        }
    }
}