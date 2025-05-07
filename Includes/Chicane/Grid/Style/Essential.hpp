#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            // Extension
            static constexpr const char* FILE_EXTENSION_NAME = "decal";

            // Attribute
            static constexpr const char* STYLE_ATTRIBUTE_NAME = "style";

            // Display
            static constexpr const char* DISPLAY_ATTRIBUTE_NAME = "display";

            // Sizes
            static constexpr const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr const char* HEIGHT_ATTRIBUTE_NAME = "height";

            // Position
            static constexpr const char* POSITION_ATTRIBUTE_NAME = "position";
            static constexpr const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static constexpr const char* POSITION_TYPE_RELATIVE  = "relative";

            // Separator
            static constexpr char ONELINE_SEPARATOR  = ' ';
            static constexpr char SELECTOR_SEPARATOR = ',';
            static constexpr char CLASS_SEPARATOR    = ' ';

            // Selector
            static constexpr char ID_SELECTOR        = '#';
            static constexpr char CLASS_SELECTOR     = '.';
            static constexpr char INCLUSIVE_SELECTOR = '*';

            // Color
            static constexpr const char* BACKGROUND_COLOR_TRANSPARENT = "transparent";
            static constexpr const char* HEX_COLOR_TRANSPARENT        = "#00000000";

            /*
            * Template 1: "`SINGLE_MARGIN`"
            * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
            * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
            * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
            */
            static constexpr const char* MARGIN_ATTRIBUTE_NAME        = "margin";
            static constexpr const char* MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
            static constexpr const char* MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
            static constexpr const char* MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
            static constexpr const char* MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

            /*
            * Template 1: "`SINGLE_GAP`"
            * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
            * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
            * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
            */
            static constexpr const char* GAP_ATTRIBUTE_NAME        = "gap";
            static constexpr const char* GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
            static constexpr const char* GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
            static constexpr const char* GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
            static constexpr const char* GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

            static constexpr const char* LIST_DIRECTION_ATTRIBUTE_NAME = "list-direction";
            static constexpr const char* LIST_DIRECTION_COLUMN         = "COLUMN";
            static constexpr const char* LIST_DIRECTION_ROW            = "ROW";

            static constexpr const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
            static constexpr const char* FOREGROUND_DEFAULT_COLOR        = "#FFFFFF";

            static constexpr const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";

            /*
            * Template 1: "`SINGLE_ALIGNMENT`"
            * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
            */
            static constexpr const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
            static constexpr const char* ALIGNMENT_START          = "START";
            static constexpr const char* ALIGNMENT_CENTER         = "CENTER";
            static constexpr const char* ALIGNMENT_END            = "END";

            // Size units
            static constexpr const char* AUTO_SIZE_UNIT            = "auto";
            static constexpr const char* PIXEL_SIZE_UNIT           = "px";
            static constexpr const char* PERCENTAGE_SIZE_UNIT      = "%";
            static constexpr const char* VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
            static constexpr const char* VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

            // Special keywords
            static constexpr const char* CALCULATION_KEYWORD = "calc";
            static constexpr const char* VARIABLE_KEYWORD    = "var";
            static constexpr const char* HEX_KEYWORD         = "#";
            static constexpr const char* RGB_KEYWORD         = "rgb";
            static constexpr const char* RGBA_KEYWORD        = "rgba";

            // Calculation
            static constexpr const char* CALCULATION_OPERATOR_SUM = "+";
            static constexpr const char* CALCULATION_OPERATOR_SUB = "-";
            static constexpr const char* CALCULATION_OPERATOR_MUL = "*";
            static constexpr const char* CALCULATION_OPERATOR_DIV = "\\";
            static const std::vector<const char*> CALCULATION_OPERATORS = {
                CALCULATION_OPERATOR_SUM,
                CALCULATION_OPERATOR_SUB,
                CALCULATION_OPERATOR_MUL,
                CALCULATION_OPERATOR_DIV
            };

            CHICANE Vec<4, std::uint32_t> toRgba(const std::string& inValue);

            CHICANE std::string variableToReference(const std::string& inValue);
            CHICANE std::string colorToReference(const std::string& inValue);
        }
    }
}