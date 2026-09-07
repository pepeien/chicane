#include "Chicane/Core/Size.hpp"

#include <cctype>
#include <cstdint>
#include <math.h>

namespace Chicane
{

    void Size::setIsAsobute(bool inValue)
    {
        m_bIsAbsolute = inValue;
    }

    void Size::setFontSize(float inValue)
    {
        m_fontSize = inValue;
    }

    void Size::setRoot(const Vec2& inValue)
    {
        m_root = inValue;
    }

    void Size::setParent(const Vec2& inValue)
    {
        m_parent = inValue;
    }

    void Size::setTextParser(TextParser inValue)
    {
        m_textParser = inValue;
    }

    float Size::parse(const String& inValue, SizeDirection inDirection) const
    {
        const String value = parseText(inValue).trim();

        if (value.startsWith(CALCULATION_KEYWORD))
        {
            return parseExpression(value, inDirection);
        }

        if (value.endsWith(EM_UNIT))
        {
            return parseEM(value);
        }

        if (value.endsWith(PERCENTAGE_UNIT))
        {
            return parsePercentage(value, inDirection);
        }

        if (value.endsWith(VIEWPORT_HEIGHT_UNIT))
        {
            return parseViewportHeight(value);
        }

        if (value.endsWith(VIEWPORT_WIDTH_UNIT))
        {
            return parseViewportWidth(value);
        }

        if (value.endsWith(PIXEL_UNIT))
        {
            return parsePixel(value);
        }

        return parseNumber(value);
    }

    String Size::parseText(const String& inValue) const
    {
        if (!m_textParser)
        {
            return inValue;
        }

        return m_textParser(inValue);
    }

    String Size::extractCalculationBody(const String& inValue) const
    {
        const std::size_t keyword = inValue.find(CALCULATION_KEYWORD);
        if (keyword == String::npos)
        {
            return String::empty();
        }

        std::size_t open = keyword;
        while (open < inValue.size() && inValue.at(open) != FUNCTION_PARAMS_OPENING)
        {
            open++;
        }

        if (open >= inValue.size() || inValue.at(open) != FUNCTION_PARAMS_OPENING)
        {
            return String::empty();
        }

        std::uint32_t depth = 0;
        for (std::size_t i = open; i < inValue.size(); i++)
        {
            const char character = inValue.at(i);

            if (character == FUNCTION_PARAMS_OPENING)
            {
                depth++;

                continue;
            }

            if (character != FUNCTION_PARAMS_CLOSING)
            {
                continue;
            }

            depth--;
            if (depth == 0)
            {
                return inValue.substr(open + 1, i - open - 1).trim();
            }
        }

        return inValue.substr(open + 1).trim();
    }

    bool isBinaryOperator(const String& inValue, std::uint32_t inIndex)
    {
        for (std::uint32_t j = inIndex; j > 0;)
        {
            j--;

            const char previous = inValue.at(j);
            if (std::isspace(static_cast<unsigned char>(previous)))
            {
                continue;
            }

            return previous != Size::FUNCTION_PARAMS_OPENING && previous != Size::CALCULATION_OPERATOR_SUM &&
                   previous != Size::CALCULATION_OPERATOR_SUB && previous != Size::CALCULATION_OPERATOR_MUL &&
                   previous != Size::CALCULATION_OPERATOR_DIV;
        }

        return false;
    }

    bool isSoleCalculation(const String& inValue)
    {
        if (!inValue.startsWith(Size::CALCULATION_KEYWORD))
        {
            return false;
        }

        std::size_t open = 0;
        while (open < inValue.size() && inValue.at(open) != Size::FUNCTION_PARAMS_OPENING)
        {
            open++;
        }

        if (open >= inValue.size())
        {
            return false;
        }

        std::uint32_t depth = 0;
        for (std::size_t i = open; i < inValue.size(); i++)
        {
            const char character = inValue.at(i);
            if (character == Size::FUNCTION_PARAMS_OPENING)
            {
                depth++;

                continue;
            }

            if (character != Size::FUNCTION_PARAMS_CLOSING)
            {
                continue;
            }

            depth--;
            if (depth == 0)
            {
                return inValue.substr(i + 1).trim().isEmpty();
            }
        }

        return false;
    }

    float Size::parseCalculation(const String& inValue, SizeDirection inDirection) const
    {
        return parseExpression(extractCalculationBody(inValue), inDirection);
    }

    float Size::parseExpression(const String& inValue, SizeDirection inDirection) const
    {
        const String value = inValue.trim();
        if (value.isEmpty())
        {
            return 0.0f;
        }

        if (isSoleCalculation(value))
        {
            return parseCalculation(value, inDirection);
        }

        std::int32_t depth = 0;
        std::int32_t split = -1;
        char         op    = 0;

        for (std::int32_t i = static_cast<std::int32_t>(value.size()) - 1; i >= 0; i--)
        {
            const char character = value.at(static_cast<std::uint32_t>(i));

            if (character == FUNCTION_PARAMS_CLOSING)
            {
                depth++;

                continue;
            }

            if (character == FUNCTION_PARAMS_OPENING)
            {
                if (depth > 0)
                {
                    depth--;
                }

                continue;
            }

            if (depth == 0 && (character == CALCULATION_OPERATOR_SUM || character == CALCULATION_OPERATOR_SUB) &&
                isBinaryOperator(value, static_cast<std::uint32_t>(i)))
            {
                split = i;
                op    = character;

                break;
            }
        }

        if (split < 0)
        {
            depth = 0;

            for (std::uint32_t i = 0; i < value.size(); i++)
            {
                const char character = value.at(i);

                if (character == FUNCTION_PARAMS_OPENING)
                {
                    depth++;

                    continue;
                }

                if (character == FUNCTION_PARAMS_CLOSING)
                {
                    if (depth > 0)
                    {
                        depth--;
                    }

                    continue;
                }

                if (depth == 0 && (character == CALCULATION_OPERATOR_MUL || character == CALCULATION_OPERATOR_DIV))
                {
                    split = static_cast<std::int32_t>(i);
                    op    = character;

                    break;
                }
            }
        }

        if (split >= 0)
        {
            const float left  = parseExpression(value.substr(0, static_cast<std::size_t>(split)), inDirection);
            const float right = parseExpression(value.substr(static_cast<std::size_t>(split) + 1), inDirection);

            if (op == CALCULATION_OPERATOR_SUM)
            {
                return left + right;
            }

            if (op == CALCULATION_OPERATOR_SUB)
            {
                return left - right;
            }

            if (op == CALCULATION_OPERATOR_MUL)
            {
                return left * right;
            }

            if (op == CALCULATION_OPERATOR_DIV)
            {
                return right == 0.0f ? 0.0f : left / right;
            }
        }

        return parse(value, inDirection);
    }

    float Size::parseEM(const String& inValue) const
    {
        if (!inValue.endsWith(EM_UNIT))
        {
            return 0.0f;
        }

        return parseEM(parseNumberUnit(inValue, EM_UNIT));
    }

    float Size::parseEM(float inValue) const
    {
        return inValue * m_fontSize;
    }

    float Size::parsePercentage(const String& inValue, SizeDirection inDirection) const
    {
        if (!inValue.endsWith(PERCENTAGE_UNIT))
        {
            return 0.0f;
        }

        return parsePercentage(parseNumberUnit(inValue, PERCENTAGE_UNIT), inDirection);
    }

    float Size::parsePercentage(float inValue, SizeDirection inDirection) const
    {
        const float value = inValue / 100;

        const Vec2& size = m_bIsAbsolute ? m_root : m_parent;

        if (inDirection == SizeDirection::Horizontal)
        {
            return std::max(0.0f, size.x) * value;
        }

        return std::max(0.0f, size.y) * value;
    }

    float Size::parseViewportHeight(const String& inValue) const
    {
        if (!inValue.endsWith(VIEWPORT_HEIGHT_UNIT))
        {
            return 0.0f;
        }

        return parseViewportHeight(parseNumberUnit(inValue, VIEWPORT_HEIGHT_UNIT));
    }

    float Size::parseViewportHeight(float inValue) const
    {
        return m_root.y * (inValue / 100.0f);
    }

    float Size::parseViewportWidth(const String& inValue) const
    {
        if (!inValue.endsWith(VIEWPORT_WIDTH_UNIT))
        {
            return 0.0f;
        }

        return parseViewportWidth(parseNumberUnit(inValue, VIEWPORT_WIDTH_UNIT));
    }

    float Size::parseViewportWidth(float inValue) const
    {
        return m_root.x * (inValue / 100.0f);
    }

    float Size::parsePixel(const String& inValue) const
    {
        if (!inValue.endsWith(PIXEL_UNIT))
        {
            return 0.0f;
        }

        return parseNumberUnit(inValue, PIXEL_UNIT);
    }

    float Size::parseNumberUnit(const String& inValue, const String& inUnit) const
    {
        if (inValue.isEmpty() || inValue.size() < inUnit.size())
        {
            return 0.0f;
        }

        return parseNumber(inValue.substr(0, inValue.size() - inUnit.size()));
    }

    float Size::parseNumber(const String& inValue) const
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
        catch (...)
        {
            return 0.0f;
        }
    }
}