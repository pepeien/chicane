#include "Chicane/Grid/Style/Border.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        float paintedWidth(float inWidth, StyleBorderType inType)
        {
            if (inType == StyleBorderType::None)
            {
                return 0.0f;
            }

            return std::max(0.0f, inWidth);
        }

        template <typename T>
        void assignOneliner(
            const std::vector<String>& inValues,
            StyleProperty<T>&          outTop,
            StyleProperty<T>&          outRight,
            StyleProperty<T>&          outBottom,
            StyleProperty<T>&          outLeft
        )
        {
            if (inValues.empty())
            {
                return;
            }

            String top    = inValues.at(0);
            String right  = top;
            String bottom = top;
            String left   = top;

            if (inValues.size() == 2)
            {
                right = inValues.at(1);
                left  = right;
            }
            else if (inValues.size() == 3)
            {
                right  = inValues.at(1);
                bottom = inValues.at(2);
                left   = right;
            }
            else if (inValues.size() >= 4)
            {
                right  = inValues.at(1);
                bottom = inValues.at(2);
                left   = inValues.at(3);
            }

            outTop.setRaw(top.trim());
            outRight.setRaw(right.trim());
            outBottom.setRaw(bottom.trim());
            outLeft.setRaw(left.trim());
        }

        StyleBorder::StyleBorder()
            : width({}),
              typeTop(StyleBorderType::None),
              typeBottom(StyleBorderType::None),
              typeLeft(StyleBorderType::None),
              typeRight(StyleBorderType::None),
              colorTop(Color::toRgba(Color::HEX_COLOR_BLACK)),
              colorBottom(Color::toRgba(Color::HEX_COLOR_BLACK)),
              colorLeft(Color::toRgba(Color::HEX_COLOR_BLACK)),
              colorRight(Color::toRgba(Color::HEX_COLOR_BLACK))
        {}

        void StyleBorder::refresh()
        {
            width.refresh();
            typeTop.refresh();
            typeBottom.refresh();
            typeLeft.refresh();
            typeRight.refresh();
            colorTop.refresh();
            colorBottom.refresh();
            colorLeft.refresh();
            colorRight.refresh();
        }

        void StyleBorder::setProperties(const StyleRuleset::Properties& inProperties)
        {
            if (inProperties.find(Style::BORDER_ATTRIBUTE_NAME) != inProperties.end())
            {
                const String& value = inProperties.at(Style::BORDER_ATTRIBUTE_NAME);

                applyShorthand(value, width.top, typeTop, colorTop);
                applyShorthand(value, width.right, typeRight, colorRight);
                applyShorthand(value, width.bottom, typeBottom, colorBottom);
                applyShorthand(value, width.left, typeLeft, colorLeft);
            }

            if (inProperties.find(Style::BORDER_WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.setOneliner(inProperties.at(Style::BORDER_WIDTH_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_STYLE_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyTypeOneliner(inProperties.at(Style::BORDER_STYLE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyColorOneliner(inProperties.at(Style::BORDER_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_TOP_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyShorthand(inProperties.at(Style::BORDER_TOP_ATTRIBUTE_NAME), width.top, typeTop, colorTop);
            }

            if (inProperties.find(Style::BORDER_RIGHT_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyShorthand(inProperties.at(Style::BORDER_RIGHT_ATTRIBUTE_NAME), width.right, typeRight, colorRight);
            }

            if (inProperties.find(Style::BORDER_BOTTOM_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyShorthand(
                    inProperties.at(Style::BORDER_BOTTOM_ATTRIBUTE_NAME),
                    width.bottom,
                    typeBottom,
                    colorBottom
                );
            }

            if (inProperties.find(Style::BORDER_LEFT_ATTRIBUTE_NAME) != inProperties.end())
            {
                applyShorthand(inProperties.at(Style::BORDER_LEFT_ATTRIBUTE_NAME), width.left, typeLeft, colorLeft);
            }

            if (inProperties.find(Style::BORDER_TOP_WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.top.setRaw(inProperties.at(Style::BORDER_TOP_WIDTH_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.right.setRaw(inProperties.at(Style::BORDER_RIGHT_WIDTH_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.bottom.setRaw(inProperties.at(Style::BORDER_BOTTOM_WIDTH_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_LEFT_WIDTH_ATTRIBUTE_NAME) != inProperties.end())
            {
                width.left.setRaw(inProperties.at(Style::BORDER_LEFT_WIDTH_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_TOP_STYLE_ATTRIBUTE_NAME) != inProperties.end())
            {
                typeTop.setRaw(inProperties.at(Style::BORDER_TOP_STYLE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_RIGHT_STYLE_ATTRIBUTE_NAME) != inProperties.end())
            {
                typeRight.setRaw(inProperties.at(Style::BORDER_RIGHT_STYLE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_BOTTOM_STYLE_ATTRIBUTE_NAME) != inProperties.end())
            {
                typeBottom.setRaw(inProperties.at(Style::BORDER_BOTTOM_STYLE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_LEFT_STYLE_ATTRIBUTE_NAME) != inProperties.end())
            {
                typeLeft.setRaw(inProperties.at(Style::BORDER_LEFT_STYLE_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_TOP_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                colorTop.setRaw(inProperties.at(Style::BORDER_TOP_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_RIGHT_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                colorRight.setRaw(inProperties.at(Style::BORDER_RIGHT_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                colorBottom.setRaw(inProperties.at(Style::BORDER_BOTTOM_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BORDER_LEFT_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                colorLeft.setRaw(inProperties.at(Style::BORDER_LEFT_COLOR_ATTRIBUTE_NAME));
            }
        }

        void StyleBorder::parseWith(WidthParser inWidthParser, ColorParser inColorParser, TypeParser inTypeParser)
        {
            width.parseWith(inWidthParser);
            typeTop.parseWith(inTypeParser);
            typeBottom.parseWith(inTypeParser);
            typeLeft.parseWith(inTypeParser);
            typeRight.parseWith(inTypeParser);
            colorTop.parseWith(inColorParser);
            colorBottom.parseWith(inColorParser);
            colorLeft.parseWith(inColorParser);
            colorRight.parseWith(inColorParser);
        }

        float StyleBorder::paintedTop() const
        {
            return paintedWidth(width.top.get(), typeTop.get());
        }

        float StyleBorder::paintedRight() const
        {
            return paintedWidth(width.right.get(), typeRight.get());
        }

        float StyleBorder::paintedBottom() const
        {
            return paintedWidth(width.bottom.get(), typeBottom.get());
        }

        float StyleBorder::paintedLeft() const
        {
            return paintedWidth(width.left.get(), typeLeft.get());
        }

        Vec4 StyleBorder::paintedWidths() const
        {
            return Vec4(paintedTop(), paintedRight(), paintedBottom(), paintedLeft());
        }

        bool StyleBorder::isVisible() const
        {
            return paintedTop() > 0.0f || paintedRight() > 0.0f || paintedBottom() > 0.0f || paintedLeft() > 0.0f;
        }

        void StyleBorder::applyShorthand(
            const String&                   inValue,
            StyleProperty<float>&           outWidth,
            StyleProperty<StyleBorderType>& outType,
            StyleProperty<Color::Rgba>&     outColor
        )
        {
            String widthValue = "0px";
            String typeValue  = Style::BORDER_STYLE_TYPE_NONE;
            String colorValue = "";
            bool   bHasWidth  = false;
            bool   bHasType   = false;
            bool   bHasColor  = false;

            for (const String& token : splitOneliner(inValue))
            {
                const String value = token.trim();

                if (value.isEmpty())
                {
                    continue;
                }

                if (isTypeToken(value))
                {
                    typeValue = value;
                    bHasType  = true;

                    continue;
                }

                if (isColorToken(value))
                {
                    colorValue = value.equals("currentcolor") ? String::empty() : value;
                    bHasColor  = true;

                    continue;
                }

                widthValue = value;
                bHasWidth  = true;
            }

            if (bHasType && !bHasWidth && !typeValue.equals(Style::BORDER_STYLE_TYPE_NONE) &&
                !typeValue.equals(Style::BORDER_STYLE_TYPE_HIDDEN))
            {
                widthValue = "3px";
            }

            outWidth.setRaw(widthValue);
            outType.setRaw(bHasType ? typeValue : Style::BORDER_STYLE_TYPE_NONE);
            outColor.setRaw(colorValue);

            (void)bHasColor;
        }

        void StyleBorder::applyTypeOneliner(const String& inValue)
        {
            assignOneliner(splitOneliner(inValue), typeTop, typeRight, typeBottom, typeLeft);
        }

        void StyleBorder::applyColorOneliner(const String& inValue)
        {
            assignOneliner(splitOneliner(inValue), colorTop, colorRight, colorBottom, colorLeft);
        }

        bool StyleBorder::isTypeToken(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            return value.equals(Style::BORDER_STYLE_TYPE_NONE) || value.equals(Style::BORDER_STYLE_TYPE_HIDDEN) ||
                   value.equals(Style::BORDER_STYLE_TYPE_SOLID) || value.equals(Style::BORDER_STYLE_TYPE_DASHED) ||
                   value.equals(Style::BORDER_STYLE_TYPE_DOTTED) || value.equals(Style::BORDER_STYLE_TYPE_DOUBLE) ||
                   value.equals(Style::BORDER_STYLE_TYPE_GROOVE) || value.equals(Style::BORDER_STYLE_TYPE_RIDGE) ||
                   value.equals(Style::BORDER_STYLE_TYPE_INSET) || value.equals(Style::BORDER_STYLE_TYPE_OUTSET);
        }

        bool StyleBorder::isColorToken(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.startsWith(Style::HEX_KEYWORD) || value.startsWith(Style::RGB_KEYWORD) ||
                value.startsWith(Style::VARIABLE_KEYWORD) || value.equals("currentcolor"))
            {
                return true;
            }

            return value.equals(Color::TEXT_COLOR_TRANSPARENT) || value.equals(Color::TEXT_COLOR_RED) ||
                   value.equals(Color::TEXT_COLOR_GREEN) || value.equals(Color::TEXT_COLOR_BLUE) ||
                   value.equals(Color::TEXT_COLOR_LIME) || value.equals(Color::TEXT_COLOR_YELLOW) ||
                   value.equals(Color::TEXT_COLOR_ORANGE) || value.equals(Color::TEXT_COLOR_CYAN) ||
                   value.equals(Color::TEXT_COLOR_WHITE) || value.equals(Color::TEXT_COLOR_GRAY) ||
                   value.equals(Color::TEXT_COLOR_BLACK);
        }
    }
}
