#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Alignment.hpp"
#include "Chicane/Grid/Style/Display.hpp"
#include "Chicane/Grid/Style/Flex/Direction.hpp"
#include "Chicane/Grid/Style/Flex/Wrap.hpp"
#include "Chicane/Grid/Style/Position.hpp"
#include "Chicane/Grid/Style/WordBreak.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct Style;

        struct CHICANE_GRID LayoutMetrics
        {
        public:
            static LayoutMetrics sCapture(const Style& inStyle);

        public:
            StyleDisplay       display;
            StylePosition      position;
            StyleAlignment     align;
            StyleFlexDirection flexDir;
            StyleFlexWrap      flexWrap;
            StyleWordBreak     wordBreak;
            String             widthRaw;
            String             heightRaw;
            String             minWidthRaw;
            String             minHeightRaw;
            String             maxWidthRaw;
            String             maxHeightRaw;
            String             marginL;
            String             marginR;
            String             marginT;
            String             marginB;
            String             paddingL;
            String             paddingR;
            String             paddingT;
            String             paddingB;
        };

        CHICANE_GRID bool operator==(const LayoutMetrics& inLeft, const LayoutMetrics& inRight);
    }
}
