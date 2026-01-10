#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleCorners
        {
        public:
            StyleCorners(
                const String& inOnelineAttributeName,
                const String& inTopAttributeName,
                const String& inBottomAttributeName,
                const String& inLeftAttributeName,
                const String& inRightAttributeName
            );

        public:
            friend bool
            operator==(const StyleCorners& inLeft, const StyleCorners& inRight)
            {
                return (
                    std::fabs(inLeft.top - inRight.top) < FLT_EPSILON &&
                    std::fabs(inLeft.bottom - inRight.bottom) < FLT_EPSILON &&
                    std::fabs(inLeft.left - inRight.left) < FLT_EPSILON &&
                    std::fabs(inLeft.right - inRight.right) < FLT_EPSILON
                );
            }

        public:
            void setAll(float inValue);

            bool refresh(
                const StyleSource::Map&                             inSource,
                std::function<float(const String&, StyleDirection)> inCalculator
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