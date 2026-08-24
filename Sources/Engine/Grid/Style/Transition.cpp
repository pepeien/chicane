#include "Chicane/Grid/Style/Transition.hpp"

#include <cstring>
#include <cstdlib>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleTransition::StyleTransition()
            : property(Style::TRANSITION_PROPERTY_ALL),
              duration(0.0f),
              delay(0.0f),
              easing(Drift::EasingCurve::ease())
        {}

        bool isTime(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.endsWith(Size::MILLISECOND_UNIT))
            {
                const String number = value.substr(0, value.size() - std::strlen(Size::MILLISECOND_UNIT)).trim();

                return !number.isEmpty() && !number.isNaN();
            }

            if (value.endsWith(Size::SECOND_UNIT))
            {
                const String number = value.substr(0, value.size() - std::strlen(Size::SECOND_UNIT)).trim();

                return !number.isEmpty() && !number.isNaN();
            }

            return !value.isEmpty() && !value.isNaN();
        }

        bool isEasing(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            return std::find(Style::EASING_TYPES.begin(), Style::EASING_TYPES.end(), value) !=
                       Style::EASING_TYPES.end() ||
                   value.startsWith(Style::EASING_CUBIC_BEZIER_KEYWORD);
        }

        float parseTime(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.endsWith(Size::MILLISECOND_UNIT))
            {
                const String number = value.substr(0, value.size() - std::strlen(Size::MILLISECOND_UNIT)).trim();

                if (number.isEmpty() || number.isNaN())
                {
                    return 0.0f;
                }

                return static_cast<float>(std::strtod(number.toChar(), nullptr));
            }

            if (value.endsWith(Size::SECOND_UNIT))
            {
                const String number = value.substr(0, value.size() - std::strlen(Size::SECOND_UNIT)).trim();

                if (number.isEmpty() || number.isNaN())
                {
                    return 0.0f;
                }

                return static_cast<float>(std::strtod(number.toChar(), nullptr) * 1000.0);
            }

            if (value.isEmpty() || value.isNaN())
            {
                return 0.0f;
            }

            return static_cast<float>(std::strtod(value.toChar(), nullptr));
        }

        Drift::EasingCurve parseEasing(const String& inValue)
        {
            return Drift::EasingCurve::fromString(inValue);
        }

        std::vector<String> splitStyleList(const String& inValue)
        {
            std::vector<String> result;
            String              current;
            int                 depth = 0;

            for (char character : inValue)
            {
                if (character == METHOD_PARAMS_OPENING)
                {
                    depth++;
                }
                else if (character == METHOD_PARAMS_CLOSING && depth > 0)
                {
                    depth--;
                }

                if (character == METHOD_PARAMS_SEPARATOR && depth == 0)
                {
                    const String token = current.trim();

                    if (!token.isEmpty())
                    {
                        result.push_back(token);
                    }

                    current = "";

                    continue;
                }

                current.append(character);
            }

            const String token = current.trim();

            if (!token.isEmpty())
            {
                result.push_back(token);
            }

            return result;
        }
    }
}
