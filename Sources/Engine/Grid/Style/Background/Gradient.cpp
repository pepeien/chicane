#include "Chicane/Grid/Style/Background/Gradient.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Size.hpp"

#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/Transition.hpp"

namespace Chicane
{
    namespace Grid
    {
        static bool parseStopOffset(const String& inValue, float& outOffset)
        {
            String value = inValue.trim().toLower();

            if (value.isEmpty())
            {
                return false;
            }

            if (value.endsWith(Size::PERCENTAGE_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Size::PERCENTAGE_UNIT)).trim();

                if (value.isEmpty() || value.isNaN())
                {
                    return false;
                }

                outOffset = std::strtof(value.toChar(), nullptr) / 100.0f;

                return true;
            }

            if (value.endsWith(Size::PIXEL_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Size::PIXEL_UNIT)).trim();
            }
            else if (value.endsWith(Size::EM_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Size::EM_UNIT)).trim();
            }

            if (value.isEmpty() || value.isNaN())
            {
                return false;
            }

            outOffset = std::strtof(value.toChar(), nullptr);

            if (outOffset > 1.0f)
            {
                outOffset /= 100.0f;
            }

            return true;
        }

        static bool isHintToken(const String& inValue)
        {
            float offset = 0.0f;

            if (!parseStopOffset(inValue, offset))
            {
                return false;
            }

            const String lower = inValue.trim().toLower();

            return !lower.startsWith("#") && !lower.startsWith(Style::RGB_KEYWORD) &&
                   !lower.startsWith(Style::RGBA_KEYWORD);
        }

        static bool extractColorToken(const String& inValue, String& outColor, String& outRest)
        {
            const String value = inValue.trim();

            if (value.isEmpty())
            {
                return false;
            }

            if (value.startsWith(Style::HEX_KEYWORD))
            {
                std::size_t end = 1;
                while (end < value.size())
                {
                    if (!std::isxdigit(static_cast<unsigned char>(value.at(end))))
                    {
                        break;
                    }

                    end++;
                }

                outColor = value.substr(0, end);
                outRest  = value.substr(end).trim();

                return true;
            }

            const String lower = value.toLower();
            if (lower.startsWith(Style::RGB_KEYWORD) || lower.startsWith(Style::RGBA_KEYWORD))
            {
                const std::size_t open = value.firstOf(METHOD_PARAMS_OPENING);
                if (open == String::npos)
                {
                    return false;
                }

                int depth = 0;
                for (std::size_t i = open; i < value.size(); i++)
                {
                    const char character = value.at(i);
                    if (character == METHOD_PARAMS_OPENING)
                    {
                        depth++;

                        continue;
                    }

                    if (character != METHOD_PARAMS_CLOSING)
                    {
                        continue;
                    }

                    depth--;
                    if (depth == 0)
                    {
                        outColor = value.substr(0, i + 1);
                        outRest  = value.substr(i + 1).trim();

                        return true;
                    }
                }

                return false;
            }

            const std::size_t space = value.firstOf(' ');
            if (space == String::npos)
            {
                outColor = value;
                outRest  = "";

                return true;
            }

            outColor = value.substr(0, space);
            outRest  = value.substr(space + 1).trim();

            return true;
        }

        static bool isLinearDirection(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.startsWith("to ") || value.equals("to"))
            {
                return true;
            }

            return value.endsWith(Style::TRANSFORM_DEGREE_UNIT) || value.endsWith(Style::TRANSFORM_RADIAN_UNIT) ||
                   value.endsWith(Style::TRANSFORM_TURN_UNIT);
        }

        static bool isRadialShape(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            return value.startsWith("circle") || value.startsWith("ellipse") || value.startsWith("at ") ||
                   value.equals("at") || value.startsWith("closest-") || value.startsWith("farthest-");
        }

        static float parseAngleDegrees(const String& inValue)
        {
            String value = inValue.trim().toLower();

            if (value.endsWith(Style::TRANSFORM_DEGREE_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Style::TRANSFORM_DEGREE_UNIT)).trim();
            }
            else if (value.endsWith(Style::TRANSFORM_RADIAN_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Style::TRANSFORM_RADIAN_UNIT)).trim();

                if (value.isEmpty() || value.isNaN())
                {
                    return 0.0f;
                }

                return std::strtof(value.toChar(), nullptr) * Math::RAD_TO_DEG;
            }
            else if (value.endsWith(Style::TRANSFORM_TURN_UNIT))
            {
                value = value.substr(0, value.size() - std::strlen(Style::TRANSFORM_TURN_UNIT)).trim();

                if (value.isEmpty() || value.isNaN())
                {
                    return 0.0f;
                }

                return std::strtof(value.toChar(), nullptr) * 360.0f;
            }

            if (value.isEmpty() || value.isNaN())
            {
                return 0.0f;
            }

            return std::strtof(value.toChar(), nullptr);
        }

        static Vec2 parseLinearAxis(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.startsWith("to"))
            {
                const bool bLeft   = value.contains("left");
                const bool bRight  = value.contains("right");
                const bool bTop    = value.contains("top");
                const bool bBottom = value.contains("bottom");

                float x = 0.0f;
                float y = 0.0f;

                if (bRight)
                {
                    x += 1.0f;
                }

                if (bLeft)
                {
                    x -= 1.0f;
                }

                if (bBottom)
                {
                    y += 1.0f;
                }

                if (bTop)
                {
                    y -= 1.0f;
                }

                if (x == 0.0f && y == 0.0f)
                {
                    y = 1.0f;
                }

                return Vec2(x, y);
            }

            const float radians = parseAngleDegrees(value) * Math::DEG_TO_RAD;

            return Vec2(std::sin(radians), -std::cos(radians));
        }

        static float parsePositionToken(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.equals("left") || value.equals("top"))
            {
                return 0.0f;
            }

            if (value.equals("center"))
            {
                return 0.5f;
            }

            if (value.equals("right") || value.equals("bottom"))
            {
                return 1.0f;
            }

            float offset = 0.5f;
            if (!parseStopOffset(value, offset))
            {
                return 0.5f;
            }

            return std::clamp(offset, 0.0f, 1.0f);
        }

        static Vec2 parseRadialCenter(const String& inValue)
        {
            Vec2 result(0.5f, 0.5f);

            const String      value = inValue.trim().toLower();
            const std::size_t at    = value.find("at ");
            if (at == String::npos)
            {
                return result;
            }

            const std::vector<String> tokens = value.substr(at + 3).trim().split(' ');
            std::vector<String>       positions;
            for (const String& token : tokens)
            {
                const String part = token.trim();
                if (part.isEmpty() || part.startsWith("closest-") || part.startsWith("farthest-") ||
                    part.equals("circle") || part.equals("ellipse"))
                {
                    continue;
                }

                positions.push_back(part);
            }

            if (positions.empty())
            {
                return result;
            }

            if (positions.size() == 1)
            {
                const String token = positions.front().toLower();
                if (token.equals("top") || token.equals("bottom"))
                {
                    result.y = parsePositionToken(token);
                }
                else if (token.equals("left") || token.equals("right"))
                {
                    result.x = parsePositionToken(token);
                }
                else
                {
                    result.x = parsePositionToken(token);
                }

                return result;
            }

            String first  = positions.at(0).toLower();
            String second = positions.at(1).toLower();
            if ((first.equals("top") || first.equals("bottom")) && (second.equals("left") || second.equals("right")))
            {
                std::swap(first, second);
            }

            result.x = parsePositionToken(first);
            result.y = parsePositionToken(second);

            return result;
        }

        static void normalizeStops(std::vector<StyleGradientStop>& outStops)
        {
            if (outStops.empty())
            {
                return;
            }

            if (outStops.front().offset < 0.0f)
            {
                outStops.front().offset = 0.0f;
            }

            if (outStops.back().offset < 0.0f)
            {
                outStops.back().offset = 1.0f;
            }

            std::size_t index = 0;
            while (index < outStops.size())
            {
                if (outStops.at(index).offset >= 0.0f)
                {
                    index++;

                    continue;
                }

                std::size_t next = index;
                while (next < outStops.size() && outStops.at(next).offset < 0.0f)
                {
                    next++;
                }

                const float start = outStops.at(index - 1).offset;
                const float end   = outStops.at(next).offset;
                const float span  = static_cast<float>(next - index + 1);

                for (std::size_t missing = index; missing < next; missing++)
                {
                    const float t               = static_cast<float>(missing - index + 1) / span;
                    outStops.at(missing).offset = start + ((end - start) * t);
                }

                index = next;
            }

            float last = 0.0f;
            for (StyleGradientStop& stop : outStops)
            {
                stop.offset = std::clamp(stop.offset, 0.0f, 1.0f);

                if (stop.offset < last)
                {
                    stop.offset = last;
                }

                last = stop.offset;
            }
        }

        bool StyleGradient::isDeclaration(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            return value.startsWith(Style::LINEAR_GRADIENT_KEYWORD) || value.startsWith(Style::RADIAL_GRADIENT_KEYWORD);
        }

        bool StyleGradient::isActive() const
        {
            return type != StyleGradientType::None && !stops.empty();
        }

        bool StyleGradient::isActive(const List& inLayers)
        {
            for (const StyleGradient& layer : inLayers)
            {
                if (layer.isActive())
                {
                    return true;
                }
            }

            return false;
        }

        StyleGradient::List StyleGradient::parseList(const String& inValue, const ColorParser& inParseColor)
        {
            List result;

            for (const String& layer : splitStyleList(inValue))
            {
                if (!isDeclaration(layer))
                {
                    continue;
                }

                StyleGradient parsed = parse(layer, inParseColor);
                if (parsed.isActive())
                {
                    result.push_back(std::move(parsed));
                }
            }

            return result;
        }

        StyleGradient StyleGradient::parse(const String& inValue, const ColorParser& inParseColor)
        {
            StyleGradient result;

            const String value = inValue.trim();
            if (value.isEmpty() || !inParseColor)
            {
                return result;
            }

            const String lower     = value.toLower();
            const bool   bIsLinear = lower.startsWith(Style::LINEAR_GRADIENT_KEYWORD);
            const bool   bIsRadial = lower.startsWith(Style::RADIAL_GRADIENT_KEYWORD);

            if (!bIsLinear && !bIsRadial)
            {
                return result;
            }

            const String body = value.getBetween(METHOD_PARAMS_OPENING, METHOD_PARAMS_CLOSING);
            if (body.isEmpty())
            {
                return result;
            }

            const std::vector<String> args = splitStyleList(body);
            if (args.empty())
            {
                return result;
            }

            result.type = bIsLinear ? StyleGradientType::Linear : StyleGradientType::Radial;
            result.axis = bIsLinear ? Vec2(0.0f, 1.0f) : Vec2(0.5f, 0.5f);

            std::size_t start = 0;
            if (bIsLinear && isLinearDirection(args.front()))
            {
                result.axis = parseLinearAxis(args.front());
                start       = 1;
            }
            else if (bIsRadial && isRadialShape(args.front()))
            {
                result.axis = parseRadialCenter(args.front());
                start       = 1;
            }

            for (std::size_t i = start; i < args.size(); i++)
            {
                const String argument = args.at(i).trim();
                if (argument.isEmpty() || isHintToken(argument))
                {
                    continue;
                }

                String color;
                String rest;
                if (!extractColorToken(argument, color, rest))
                {
                    continue;
                }

                StyleGradientStop stop;
                stop.color = inParseColor(color);

                const std::vector<String> offsets    = rest.split(' ');
                bool                      bHasOffset = false;
                for (const String& token : offsets)
                {
                    float offset = 0.0f;
                    if (!parseStopOffset(token, offset))
                    {
                        continue;
                    }

                    if (!bHasOffset)
                    {
                        stop.offset = offset;
                        bHasOffset  = true;

                        continue;
                    }

                    result.stops.push_back(stop);
                    stop.offset = offset;
                }

                result.stops.push_back(stop);
            }

            if (result.stops.size() > MAX_STOPS)
            {
                StyleGradientStop last = result.stops.back();
                result.stops.resize(MAX_STOPS - 1);
                result.stops.push_back(last);
            }

            if (result.stops.size() < 2)
            {
                result = {};

                return result;
            }

            normalizeStops(result.stops);

            return result;
        }
    }
}
