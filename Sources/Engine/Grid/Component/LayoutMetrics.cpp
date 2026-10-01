#include "Chicane/Grid/Component/LayoutMetrics.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        LayoutMetrics LayoutMetrics::sCapture(const Style& inStyle)
        {
            return {inStyle.display.get(),          inStyle.position.get(),         inStyle.align.get(),
                    inStyle.flex.direction.get(),   inStyle.flex.wrap.get(),        inStyle.wordBreak.get(),
                    inStyle.width.value.getRaw(),   inStyle.height.value.getRaw(),  inStyle.width.min.getRaw(),
                    inStyle.height.min.getRaw(),    inStyle.width.max.getRaw(),     inStyle.height.max.getRaw(),
                    inStyle.margin.left.getRaw(),   inStyle.margin.right.getRaw(),  inStyle.margin.top.getRaw(),
                    inStyle.margin.bottom.getRaw(), inStyle.padding.left.getRaw(),  inStyle.padding.right.getRaw(),
                    inStyle.padding.top.getRaw(),   inStyle.padding.bottom.getRaw()};
        }

        bool operator==(const LayoutMetrics& inLeft, const LayoutMetrics& inRight)
        {
            return inLeft.display == inRight.display && inLeft.position == inRight.position &&
                   inLeft.align == inRight.align && inLeft.flexDir == inRight.flexDir &&
                   inLeft.flexWrap == inRight.flexWrap && inLeft.wordBreak == inRight.wordBreak &&
                   inLeft.widthRaw.equals(inRight.widthRaw) && inLeft.heightRaw.equals(inRight.heightRaw) &&
                   inLeft.minWidthRaw.equals(inRight.minWidthRaw) && inLeft.minHeightRaw.equals(inRight.minHeightRaw) &&
                   inLeft.maxWidthRaw.equals(inRight.maxWidthRaw) && inLeft.maxHeightRaw.equals(inRight.maxHeightRaw) &&
                   inLeft.marginL.equals(inRight.marginL) && inLeft.marginR.equals(inRight.marginR) &&
                   inLeft.marginT.equals(inRight.marginT) && inLeft.marginB.equals(inRight.marginB) &&
                   inLeft.paddingL.equals(inRight.paddingL) && inLeft.paddingR.equals(inRight.paddingR) &&
                   inLeft.paddingT.equals(inRight.paddingT) && inLeft.paddingB.equals(inRight.paddingB);
        }
    }
}
