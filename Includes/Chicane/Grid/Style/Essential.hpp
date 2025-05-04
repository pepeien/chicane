#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            static constexpr const char* FILE_EXTENSION_NAME = "decal";

            static constexpr const char* STYLE_ATTRIBUTE_NAME = "style";

            static constexpr const char* DISPLAY_ATTRIBUTE_NAME = "display";

            static constexpr const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr const char* HEIGHT_ATTRIBUTE_NAME = "height";

            static constexpr const char* POSITION_ATTRIBUTE_NAME = "position";
            static constexpr const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static constexpr const char* POSITION_TYPE_RELATIVE  = "relative";

            static constexpr char ONELINE_SEPARATOR  = ' ';
            static constexpr char SELECTOR_SEPARATOR = ',';
            static constexpr char CLASS_SEPARATOR    = ' ';

            static constexpr char ID_SELECTOR        = '#';
            static constexpr char CLASS_SELECTOR     = '.';
            static constexpr char INCLUSIVE_SELECTOR = '*';

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

            static constexpr const char* BACKGROUND_COLOR_TRANSPARENT = "transparent";
            static constexpr const char* HEX_COLOR_TRANSPARENT        = "#00000000";
        }
    }
}