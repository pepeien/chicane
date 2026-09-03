#include "Chicane/Grid/Style/Radius.hpp"

#include <algorithm>
#include <cstdint>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        bool splitRadiusSides(const String& inValue, String& outHorizontal, String& outVertical)
        {
            std::uint32_t parenthesisCount = 0;

            for (std::uint32_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);

                if (character == METHOD_PARAMS_OPENING)
                {
                    parenthesisCount++;

                    continue;
                }

                if (character == METHOD_PARAMS_CLOSING)
                {
                    parenthesisCount--;

                    continue;
                }

                if (character != Style::RADIUS_SEPARATOR || parenthesisCount > 0)
                {
                    continue;
                }

                outHorizontal = inValue.substr(0, i).trim();
                outVertical   = inValue.substr(i + 1).trim();

                return true;
            }

            outHorizontal = inValue.trim();
            outVertical   = String::empty();

            return false;
        }

        StyleRadius::StyleRadius()
            : x({}),
              y({})
        {}

        void StyleRadius::refresh()
        {
            x.refresh();
            y.refresh();
        }

        void StyleRadius::constrain(float inWidth, float inHeight)
        {
            const float topLeftX     = x.top.get();
            const float topRightX    = x.right.get();
            const float bottomRightX = x.bottom.get();
            const float bottomLeftX  = x.left.get();
            const float topLeftY     = y.top.get();
            const float topRightY    = y.right.get();
            const float bottomRightY = y.bottom.get();
            const float bottomLeftY  = y.left.get();

            float factor = 1.0f;

            const auto consider = [&factor](float inSum, float inEdge)
            {
                if (inSum <= 0.0f || inEdge <= 0.0f)
                {
                    if (inSum > 0.0f && inEdge <= 0.0f)
                    {
                        factor = 0.0f;
                    }

                    return;
                }

                factor = std::min(factor, inEdge / inSum);
            };

            consider(topLeftX + topRightX, inWidth);
            consider(bottomLeftX + bottomRightX, inWidth);
            consider(topLeftY + bottomLeftY, inHeight);
            consider(topRightY + bottomRightY, inHeight);

            if (factor >= 1.0f)
            {
                return;
            }

            x.top.set(topLeftX * factor);
            x.right.set(topRightX * factor);
            x.bottom.set(bottomRightX * factor);
            x.left.set(bottomLeftX * factor);
            y.top.set(topLeftY * factor);
            y.right.set(topRightY * factor);
            y.bottom.set(bottomRightY * factor);
            y.left.set(bottomLeftY * factor);
        }

        void StyleRadius::setProperties(
            const StyleRuleset::Properties& inProperties,
            const String&                   inOnelineAttributeName,
            const String&                   inTopLeftAttributeName,
            const String&                   inTopRightAttributeName,
            const String&                   inBottomRightAttributeName,
            const String&                   inBottomLeftAttributeName
        )
        {
            if (inProperties.find(inOnelineAttributeName) != inProperties.end())
            {
                setOnelinerAttribute(inProperties.at(inOnelineAttributeName));
            }

            if (inProperties.find(inTopLeftAttributeName) != inProperties.end())
            {
                setCornerRaw(x.top, y.top, inProperties.at(inTopLeftAttributeName));
            }

            if (inProperties.find(inTopRightAttributeName) != inProperties.end())
            {
                setCornerRaw(x.right, y.right, inProperties.at(inTopRightAttributeName));
            }

            if (inProperties.find(inBottomRightAttributeName) != inProperties.end())
            {
                setCornerRaw(x.bottom, y.bottom, inProperties.at(inBottomRightAttributeName));
            }

            if (inProperties.find(inBottomLeftAttributeName) != inProperties.end())
            {
                setCornerRaw(x.left, y.left, inProperties.at(inBottomLeftAttributeName));
            }
        }

        void StyleRadius::parseWith(std::function<float(const String&, SizeDirection)> inParser)
        {
            x.parseWith(inParser, SizeDirection::Horizontal);
            y.parseWith(inParser, SizeDirection::Vertical);
        }

        Vec4 StyleRadius::horizontal() const
        {
            return x.values();
        }

        Vec4 StyleRadius::vertical() const
        {
            return y.values();
        }

        bool StyleRadius::isZero() const
        {
            return x.isZero() && y.isZero();
        }

        void StyleRadius::setOnelinerAttribute(const String& inValue)
        {
            String horizontalSide = String::empty();
            String verticalSide   = String::empty();
            splitRadiusSides(inValue, horizontalSide, verticalSide);

            x.setOnelinerAttribute(horizontalSide);

            if (verticalSide.isEmpty())
            {
                y.top.setRaw(x.top.getRaw());
                y.right.setRaw(x.right.getRaw());
                y.bottom.setRaw(x.bottom.getRaw());
                y.left.setRaw(x.left.getRaw());

                return;
            }

            y.setOnelinerAttribute(verticalSide);
        }

        void StyleRadius::setCornerRaw(
            StyleProperty<float>& outHorizontal, StyleProperty<float>& outVertical, const String& inValue
        )
        {
            const std::vector<String> values = splitOneliner(inValue);

            if (values.empty())
            {
                return;
            }

            outHorizontal.setRaw(values.at(0).trim());
            outVertical.setRaw(values.size() >= 2 ? values.at(1).trim() : values.at(0).trim());
        }
    }
}
