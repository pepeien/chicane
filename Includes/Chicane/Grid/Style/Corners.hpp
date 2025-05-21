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
                    const std::string& inOnelineAttributeName,
                    const std::string& inTopAttributeName,
                    const std::string& inBottomAttributeName,
                    const std::string& inLeftAttributeName,
                    const std::string& inRightAttributeName
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
                    std::function<float (const std::string&, Direction)> inCalculator
                );

            public:
                float top;
                float bottom;
                float left;
                float right;

            private:
                std::string m_onelineAttributeName;
                std::string m_topAttributeName;
                std::string m_bottomAttributeName;
                std::string m_leftAttributeName;
                std::string m_rightAttributeName;
            };
        }
    }
}