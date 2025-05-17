#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            // Extension
            static constexpr const char* FILE_EXTENSION_NAME = "decal";

            // Attribute
            static constexpr const char* ATTRIBUTE_NAME = "style";

            // Size units
            static constexpr const char* AUTO_SIZE_UNIT            = "auto";
            static constexpr const char* PIXEL_SIZE_UNIT           = "px";
            static constexpr const char* PERCENTAGE_SIZE_UNIT      = "%";
            static constexpr const char* VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
            static constexpr const char* VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

            // Special keywords
            static constexpr const char* CALCULATION_KEYWORD = "calc";
            static constexpr const char* VARIABLE_KEYWORD    = "var";
            static constexpr char        HEX_KEYWORD         = '#';
            static constexpr const char* RGB_KEYWORD         = "rgb";
            static constexpr const char* RGBA_KEYWORD        = "rgba";

            // Calculation
            static constexpr char CALCULATION_OPERATOR_SUM = '+';
            static constexpr char CALCULATION_OPERATOR_SUB = '-';
            static constexpr char CALCULATION_OPERATOR_MUL = '*';
            static constexpr char CALCULATION_OPERATOR_DIV = '/';
            static const std::vector<char> CALCULATION_OPERATORS = {
                CALCULATION_OPERATOR_SUM,
                CALCULATION_OPERATOR_SUB,
                CALCULATION_OPERATOR_MUL,
                CALCULATION_OPERATOR_DIV
            };

            // Separator
            static constexpr char ONELINE_SEPARATOR  = ' ';
            static constexpr char SELECTOR_SEPARATOR = ',';
            static constexpr char CLASS_SEPARATOR    = ' ';

            // Selectors
            static constexpr char ID_SELECTOR        = '#';
            static constexpr char CLASS_SELECTOR     = '.';
            static constexpr char INCLUSIVE_SELECTOR = '*';

            // Display
            static constexpr const char* DISPLAY_ATTRIBUTE_NAME = "display";
            static constexpr const char* DISPLAY_TYPE_FLEX      = "flex";
            static constexpr const char* DISPLAY_TYPE_BLOCK     = "block";
            static constexpr const char* DISPLAY_TYPE_HIDDEN    = "hidden";
            static constexpr const char* DISPLAY_TYPE_NONE      = "none";

            // Flex
            static constexpr const char* FLEX_DIRECTION_ATTRIBUTE_NAME = "flex-direction";
            static constexpr const char* FLEX_DIRECTION_TYPE_ROW       = "row";
            static constexpr const char* FLEX_DIRECTION_TYPE_COLUMN    = "column";

            // Size
            static constexpr const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr const char* HEIGHT_ATTRIBUTE_NAME = "height";

            // Position
            static constexpr const char* POSITION_ATTRIBUTE_NAME = "position";
            static constexpr const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static constexpr const char* POSITION_TYPE_RELATIVE  = "relative";

            // Color
            static constexpr const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";
            static constexpr const char* BACKGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_TRANSPARENT;

            static constexpr const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
            static constexpr const char* FOREGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_WHITE;

            // Corner
            static constexpr const char* CORNER_DEFAULT_VALUE = "0px";

            // Text
            static constexpr const char* FONT_FAMILY_ATTRIBUTE_NAME = "font-family";
            static constexpr const char* FONT_FAMILY_DEFAULT_VALUE  = "Kenney Future";

            static constexpr const char* FONT_SIZE_ATTRIBUTE_NAME = "font-size";
            static constexpr const char* FONT_SIZE_DEFAULT_VALUE  = "16px";

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

            /*
            * Template 1: "`SINGLE_ALIGNMENT`"
            * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
            */
            static constexpr const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
            static constexpr const char* ALIGNMENT_TYPE_START     = "start";
            static constexpr const char* ALIGNMENT_TYPE_CENTER    = "center";
            static constexpr const char* ALIGNMENT_TYPE_END       = "end";

            CHICANE_GRID std::string variableToReference(const std::string& inValue);
            CHICANE_GRID std::string colorToReference(const std::string& inValue);

            CHICANE_GRID std::vector<std::string> splitOneliner(const std::string& inValue);
        }
    }
}