#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Size/Direction.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE Size
    {
    public:
        using TextParser = std::function<String(const String&)>;

    public:
        // Units
        static constexpr inline const char*   AUTO_UNIT            = "auto";
        static constexpr inline const char*   PIXEL_UNIT           = "px";
        static constexpr inline const char*   EM_UNIT              = "em";
        static constexpr inline const char*   PERCENTAGE_UNIT      = "%";
        static constexpr inline const char*   VIEWPORT_HEIGHT_UNIT = "vh";
        static constexpr inline const char*   VIEWPORT_WIDTH_UNIT  = "vw";

        // Function
        static constexpr inline const char    FUNCTION_PARAMS_OPENING   = '(';
        static constexpr inline const char    FUNCTION_PARAMS_SEPARATOR = ',';
        static constexpr inline const char    FUNCTION_PARAMS_CLOSING   = ')';

        // Calculation
        static constexpr inline const char*   CALCULATION_KEYWORD      = "calc";
        static constexpr inline const char    CALCULATION_OPERATOR_SUM = '+';
        static constexpr inline const char    CALCULATION_OPERATOR_SUB = '-';
        static constexpr inline const char    CALCULATION_OPERATOR_MUL = '*';
        static constexpr inline const char    CALCULATION_OPERATOR_DIV = '/';
        static inline const std::vector<char> CALCULATION_OPERATORS    = {
            CALCULATION_OPERATOR_SUM, CALCULATION_OPERATOR_SUB, CALCULATION_OPERATOR_MUL, CALCULATION_OPERATOR_DIV
        };

    public:
        void setIsAsobute(bool inValue);
        void setFontSize(float inValue);
        void setRoot(const Vec2& inValue);
        void setParent(const Vec2& inValue);
        void setTextParser(TextParser inValue);

        float parse(const String& inValue, SizeDirection inDirection) const;

    private:
        String parseText(const String& inValue) const;

        float parseCalculation(const String& inValue, SizeDirection inDirection) const;

        float parseEM(const String& inValue) const;
        float parseEM(float inValue) const;

        float parsePercentage(const String& inValue, SizeDirection inDirection) const;
        float parsePercentage(float inValue, SizeDirection inDirection) const;

        float parseViewportHeight(const String& inValue) const;
        float parseViewportHeight(float inValue) const;

        float parseViewportWidth(const String& inValue) const;
        float parseViewportWidth(float inValue) const;

        float parsePixel(const String& inValue) const;

        float parseNumberUnit(const String& inValue, const String& inUnit) const;
        float parseNumber(const String& inValue) const;

    private:
        bool       m_bIsAbsolute = false;

        float      m_fontSize = 16.0f;

        Vec2       m_root   = Vec2::Zero;
        Vec2       m_parent = Vec2::Zero;

        TextParser m_textParser = nullptr;
    };
}