#pragma once

#include <cfloat>
#include <cmath>
#include <functional>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleCorners
        {
        public:
            StyleCorners();

        public:
            friend inline bool operator==(const StyleCorners& inLeft, const StyleCorners& inRight)
            {
                return (
                    std::fabs(inLeft.top.get() - inRight.top.get()) < FLT_EPSILON &&
                    std::fabs(inLeft.bottom.get() - inRight.bottom.get()) < FLT_EPSILON &&
                    std::fabs(inLeft.left.get() - inRight.left.get()) < FLT_EPSILON &&
                    std::fabs(inLeft.right.get() - inRight.right.get()) < FLT_EPSILON
                );
            }

        public:
            void refresh();

            void setProperties(
                const StyleRuleset::Properties& inProperties,
                const String&                   inOnelineAttributeName,
                const String&                   inTopAttributeName,
                const String&                   inBottomAttributeName,
                const String&                   inLeftAttributeName,
                const String&                   inRightAttributeName
            );

            void parseWith(std::function<float(const String&, StyleDirection)> inParser);

            void setAll(float inValue);

        private:
            void setOnelinerAttribute(const String& inValue);

        public:
            StyleProperty<float> top;
            StyleProperty<float> bottom;
            StyleProperty<float> left;
            StyleProperty<float> right;
        };
    }
}