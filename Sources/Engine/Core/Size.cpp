#include "Chicane/Core/Size.hpp"

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
        String value = parseText(inValue);

        if (value.startsWith(CALCULATION_KEYWORD))
        {
            return parseCalculation(value, inDirection);
        }

        if (value.equals(AUTO_UNIT))
        {
            return parsePercentage("100%", inDirection);
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

    float Size::parseCalculation(const String& inValue, SizeDirection inDirection) const
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

            const auto& iterator = std::find(CALCULATION_OPERATORS.begin(), CALCULATION_OPERATORS.end(), character);

            if (iterator == CALCULATION_OPERATORS.end() || parathesisCount > 0)
            {
                continue;
            }

            const float left  = parse(operation.substr(0, i), inDirection);
            const float right = parse(operation.substr(i + 1), inDirection);

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