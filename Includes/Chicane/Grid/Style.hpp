#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Alignment.hpp"
#include "Chicane/Grid/Style/Background.hpp"
#include "Chicane/Grid/Style/Corners.hpp"
#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Display.hpp"
#include "Chicane/Grid/Style/Flex.hpp"
#include "Chicane/Grid/Style/Font.hpp"
#include "Chicane/Grid/Style/Position.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        struct CHICANE_GRID Style
        {
        public:
            // Extension
            static constexpr inline const char*   FILE_EXTENSION_NAME = "decal";

            // Attribute
            static constexpr inline const char*   ATTRIBUTE_NAME = "style";

            // Size units
            static constexpr inline const char*   AUTO_SIZE_UNIT            = "auto";
            static constexpr inline const char*   PIXEL_SIZE_UNIT           = "px";
            static constexpr inline const char*   EM_SIZE_UNIT              = "em";
            static constexpr inline const char*   PERCENTAGE_SIZE_UNIT      = "%";
            static constexpr inline const char*   VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
            static constexpr inline const char*   VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

            // Special keywords
            static constexpr inline const char*   CALCULATION_KEYWORD = "calc";
            static constexpr inline const char*   VARIABLE_KEYWORD    = "var";
            static constexpr inline const char    HEX_KEYWORD         = '#';
            static constexpr inline const char*   RGB_KEYWORD         = "rgb";
            static constexpr inline const char*   RGBA_KEYWORD        = "rgba";

            // Calculation
            static constexpr inline const char    CALCULATION_OPERATOR_SUM = '+';
            static constexpr inline const char    CALCULATION_OPERATOR_SUB = '-';
            static constexpr inline const char    CALCULATION_OPERATOR_MUL = '*';
            static constexpr inline const char    CALCULATION_OPERATOR_DIV = '/';
            static inline const std::vector<char> CALCULATION_OPERATORS    = {
                CALCULATION_OPERATOR_SUM, CALCULATION_OPERATOR_SUB, CALCULATION_OPERATOR_MUL, CALCULATION_OPERATOR_DIV
            };

            // Separator
            static constexpr inline const char  ONELINE_SEPARATOR  = ' ';
            static constexpr inline const char  SELECTOR_SEPARATOR = ',';
            static constexpr inline const char  CLASS_SEPARATOR    = ' ';

            // Selectors
            static constexpr inline const char  ID_SELECTOR        = '#';
            static constexpr inline const char  CLASS_SELECTOR     = '.';
            static constexpr inline const char  INCLUSIVE_SELECTOR = '*';

            // Display
            static constexpr inline const char* DISPLAY_ATTRIBUTE_NAME = "display";
            static constexpr inline const char* DISPLAY_TYPE_FLEX      = "flex";
            static constexpr inline const char* DISPLAY_TYPE_BLOCK     = "block";
            static constexpr inline const char* DISPLAY_TYPE_HIDDEN    = "hidden";
            static constexpr inline const char* DISPLAY_TYPE_NONE      = "none";

            // Z-Index
            static constexpr inline const char* Z_INDEX_ATTRIBUTE_NAME = "z-index";

            // Flex
            static constexpr inline const char* FLEX_DIRECTION_ATTRIBUTE_NAME = "flex-direction";
            static constexpr inline const char* FLEX_DIRECTION_TYPE_ROW       = "row";
            static constexpr inline const char* FLEX_DIRECTION_TYPE_COLUMN    = "column";

            // Size
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME = "height";

            // Position
            static constexpr inline const char* POSITION_ATTRIBUTE_NAME = "position";
            static constexpr inline const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static constexpr inline const char* POSITION_TYPE_RELATIVE  = "relative";

            // Background
            static constexpr inline const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";
            static constexpr inline const char* BACKGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_TRANSPARENT;

            static constexpr inline const char* BACKGROUND_IMAGE_ATTRIBUTE_NAME = "background-image";
            static constexpr inline const char* BACKGROUND_IMAGE_DEFAULT_COLOR  = "";

            // Color
            static constexpr inline const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
            static constexpr inline const char* FOREGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_WHITE;

            static constexpr inline const char* OPACITY_ATTRIBUTE_NAME = "opacity";
            static constexpr inline const float OPACITY_DEFAULT_VALUE  = 1.0f;

            // Corner
            static constexpr inline const char* CORNER_DEFAULT_VALUE = "0px";

            // Text
            static constexpr inline const char* FONT_FAMILY_ATTRIBUTE_NAME = "font-family";
            static constexpr inline const char* FONT_FAMILY_DEFAULT_VALUE  = "Poppins";

            static constexpr inline const char* FONT_SIZE_ATTRIBUTE_NAME = "font-size";
            static constexpr inline const char* FONT_SIZE_DEFAULT_VALUE  = "16px";

            static constexpr inline const char* LETTER_SPACING_ATTRIBUTE_NAME = "letter-spacing";

            /*
             * Template 1: "`SINGLE_MARGIN`"
             * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
             * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
             * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
             */
            static constexpr inline const char* MARGIN_ATTRIBUTE_NAME        = "margin";
            static constexpr inline const char* MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
            static constexpr inline const char* MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
            static constexpr inline const char* MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
            static constexpr inline const char* MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

            /*
             * Template 1: "`SINGLE_PADDING`"
             * Template 2: "`VERTICAL_PADDING` `HORIZONTAL_PADDING`"
             * Template 3: "`TOP_PADDING` `BOTTOM_PADDING` `HORIZONTAL_PADDING`"
             * Template 4: "`TOP_PADDING` `RIGHT_PADDING` `BOTTOM_PADDING` `LEFT_PADDING`"
             */
            static constexpr inline const char* PADDING_ATTRIBUTE_NAME        = "padding";
            static constexpr inline const char* PADDING_TOP_ATTRIBUTE_NAME    = "padding-top";
            static constexpr inline const char* PADDING_BOTTOM_ATTRIBUTE_NAME = "padding-bottom";
            static constexpr inline const char* PADDING_LEFT_ATTRIBUTE_NAME   = "padding-left";
            static constexpr inline const char* PADDING_RIGHT_ATTRIBUTE_NAME  = "padding-right";

            /*
             * Template 1: "`SINGLE_GAP`"
             * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
             * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
             * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
             */
            static constexpr inline const char* GAP_ATTRIBUTE_NAME        = "gap";
            static constexpr inline const char* GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
            static constexpr inline const char* GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
            static constexpr inline const char* GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
            static constexpr inline const char* GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

            /*
             * Template 1: "`SINGLE_ALIGNMENT`"
             * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
             */
            static constexpr inline const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
            static constexpr inline const char* ALIGNMENT_TYPE_START     = "start";
            static constexpr inline const char* ALIGNMENT_TYPE_CENTER    = "center";
            static constexpr inline const char* ALIGNMENT_TYPE_END       = "end";

        public:
            static StyleSource::List parseSources(const pugi::xml_node& inNode);
            static StyleSource::List parseSources(const FileSystem::Path& inPath);
            static StyleSource::List parseSources(const String& inData);

            static StyleSource::Map parseSource(const String& inData);

        public:
            Style(const StyleSource::Map& inProperties, Component* inParent);
            Style();

        public:
            bool isDisplay(StyleDisplay inValue) const;
            bool isPosition(StylePosition inValue) const;

            void setProperties(const StyleSource::Map& inProperties);

            bool hasParent() const;
            void setParent(Component* inComponent);

            void refresh();

        private:
            void refreshDisplay();
            void refreshFlex();
            void refreshZIndex();
            void refreshSize();
            void refreshPosition();
            void refreshAlignment();
            void refreshMargin();
            void refreshPadding();
            void refreshGap();
            void refreshBackground();
            void refreshForegroundColor();
            void refreshOpacity();
            void refreshFont();
            void refreshLetterSpacing();

            Color::Rgba parseColor(const String& inValue) const;
            float parseSize(const String& inValue, StyleDirection inDirection) const;
            String parseText(const String& inValue) const;

            float parseCalculation(const String& inValue, StyleDirection inDirection) const;

            float parseEM(const String& inValue) const;
            float parseEM(float inValue) const;

            float parsePercentage(const String& inValue, StyleDirection inDirection) const;
            float parsePercentage(float inValue, StyleDirection inDirection) const;

            float parseViewportHeight(const String& inValue) const;
            float parseViewportHeight(float inValue) const;

            float parseViewportWidth(const String& inValue) const;
            float parseViewportWidth(float inValue) const;

            float parsePixel(const String& inValue) const;

            float parseNumberUnit(const String& inValue, const String& inUnit) const;
            float parseNumber(const String& inValue) const;

        public:
            // Visiblity
            StyleProperty<StyleDisplay>   display;
            StyleProperty<float>          zIndex; // [0.0f, 999.9f]

            // Size
            StyleProperty<float>          width;
            StyleProperty<float>          height;

            // Flex
            StyleFlex                     flex;

            // Positioning
            StyleProperty<StylePosition>  position;
            StyleProperty<StyleAlignment> align;
            StyleCorners                  margin;
            StyleCorners                  padding;
            StyleCorners                  gap;

            // Background
            StyleBackground               background;

            // Color
            StyleProperty<Color::Rgba>    foregroundColor;
            StyleProperty<float>          opacity;

            // Text
            StyleFont                     font;
            StyleProperty<float>          letterSpacing;

        private:
            Component* m_parent;
        };

        CHICANE_GRID String variableToReference(const String& inValue);
        CHICANE_GRID String colorToReference(const String& inValue);

        CHICANE_GRID std::vector<String> splitOneliner(const String& inValue);
    }
}