#pragma once

#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Essential.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        namespace Style
        {
            struct CHICANE_GRID Corners
            {
            public:
                Corners(
                    const String& inOnelineAttributeName,
                    const String& inTopAttributeName,
                    const String& inBottomAttributeName,
                    const String& inLeftAttributeName,
                    const String& inRightAttributeName
                );

            public:
                friend bool operator==(const Corners& inLeft, const Corners& inRight)
                {
                    return (
                        std::fabs(inLeft.top    - inRight.top)    < FLT_EPSILON &&
                        std::fabs(inLeft.bottom - inRight.bottom) < FLT_EPSILON &&
                        std::fabs(inLeft.left   - inRight.left)   < FLT_EPSILON &&
                        std::fabs(inLeft.right  - inRight.right)  < FLT_EPSILON
                    );
                }

            public:
                bool refresh(
                    const Properties &inSource,
                    std::function<float (const String&, Direction)> inCalculator
                );

            public:
                float top;
                float bottom;
                float left;
                float right;

            private:
                String m_onelineAttributeName;
                String m_topAttributeName;
                String m_bottomAttributeName;
                String m_leftAttributeName;
                String m_rightAttributeName;
            };
        }
    }
}