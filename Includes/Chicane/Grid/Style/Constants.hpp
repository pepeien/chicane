#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            // Extension
            static inline constexpr const char* FILE_EXTENSION_NAME = "decal";

            // Attribute
            static inline constexpr const char* ATTRIBUTE_NAME = "style";

            // Size units
            static inline constexpr const char* AUTO_SIZE_UNIT            = "auto";
            static inline constexpr const char* PIXEL_SIZE_UNIT           = "px";
            static inline constexpr const char* PERCENTAGE_SIZE_UNIT      = "%";
            static inline constexpr const char* VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
            static inline constexpr const char* VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

            // Special keywords
            static inline constexpr const char* CALCULATION_KEYWORD = "calc";
            static inline constexpr const char* VARIABLE_KEYWORD    = "var";
            static inline constexpr const char  HEX_KEYWORD         = '#';
            static inline constexpr const char* RGB_KEYWORD         = "rgb";
            static inline constexpr const char* RGBA_KEYWORD        = "rgba";

            // Calculation
            static inline constexpr const char CALCULATION_OPERATOR_SUM = '+';
            static inline constexpr const char CALCULATION_OPERATOR_SUB = '-';
            static inline constexpr const char CALCULATION_OPERATOR_MUL = '*';
            static inline constexpr const char CALCULATION_OPERATOR_DIV = '/';
            static const std::vector<char> CALCULATION_OPERATORS = {
                CALCULATION_OPERATOR_SUM,
                CALCULATION_OPERATOR_SUB,
                CALCULATION_OPERATOR_MUL,
                CALCULATION_OPERATOR_DIV
            };

            // Separator
            static inline constexpr const char ONELINE_SEPARATOR  = ' ';
            static inline constexpr const char SELECTOR_SEPARATOR = ',';
            static inline constexpr const char CLASS_SEPARATOR    = ' ';

            // Selectors
            static inline constexpr const char ID_SELECTOR        = '#';
            static inline constexpr const char CLASS_SELECTOR     = '.';
            static inline constexpr const char INCLUSIVE_SELECTOR = '*';

            // Display
            static inline constexpr const char* DISPLAY_ATTRIBUTE_NAME = "display";
            static inline constexpr const char* DISPLAY_TYPE_FLEX      = "flex";
            static inline constexpr const char* DISPLAY_TYPE_BLOCK     = "block";
            static inline constexpr const char* DISPLAY_TYPE_HIDDEN    = "hidden";
            static inline constexpr const char* DISPLAY_TYPE_NONE      = "none";

            // Flex
            static inline constexpr const char* FLEX_DIRECTION_ATTRIBUTE_NAME = "flex-direction";
            static inline constexpr const char* FLEX_DIRECTION_TYPE_ROW       = "row";
            static inline constexpr const char* FLEX_DIRECTION_TYPE_COLUMN    = "column";

            // Size
            static inline constexpr const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static inline constexpr const char* HEIGHT_ATTRIBUTE_NAME = "height";

            // Position
            static inline constexpr const char* POSITION_ATTRIBUTE_NAME = "position";
            static inline constexpr const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static inline constexpr const char* POSITION_TYPE_RELATIVE  = "relative";

            // Color
            static inline constexpr const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";
            static inline constexpr const char* BACKGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_TRANSPARENT;

            static inline constexpr const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
            static inline constexpr const char* FOREGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_WHITE;

            // Corner
            static inline constexpr const char* CORNER_DEFAULT_VALUE = "0px";

            // Text
            static inline constexpr const char* FONT_FAMILY_ATTRIBUTE_NAME = "font-family";
            static inline constexpr const char* FONT_FAMILY_DEFAULT_VALUE  = "Poppins";

            static inline constexpr const char* FONT_SIZE_ATTRIBUTE_NAME = "font-size";
            static inline constexpr const char* FONT_SIZE_DEFAULT_VALUE  = "16px";

            /*
            * Template 1: "`SINGLE_MARGIN`"
            * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
            * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
            * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
            */
            static inline constexpr const char* MARGIN_ATTRIBUTE_NAME        = "margin";
            static inline constexpr const char* MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
            static inline constexpr const char* MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
            static inline constexpr const char* MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
            static inline constexpr const char* MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

            /*
            * Template 1: "`SINGLE_GAP`"
            * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
            * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
            * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
            */
            static inline constexpr const char* GAP_ATTRIBUTE_NAME        = "gap";
            static inline constexpr const char* GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
            static inline constexpr const char* GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
            static inline constexpr const char* GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
            static inline constexpr const char* GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

            /*
            * Template 1: "`SINGLE_ALIGNMENT`"
            * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
            */
            static inline constexpr const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
            static inline constexpr const char* ALIGNMENT_TYPE_START     = "start";
            static inline constexpr const char* ALIGNMENT_TYPE_CENTER    = "center";
            static inline constexpr const char* ALIGNMENT_TYPE_END       = "end";
        }
    }
}