#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Flex/Direction.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFlex
        {
        public:
            using DirectionParser = StyleProperty<StyleFlexDirection>::Parser;

        public:
            StyleFlex();

        public:
            void refresh();

            void setProperties(const StyleRuleset::Properties& inProperties);

            void parseWith(DirectionParser directionParser);

        public:
            StyleProperty<StyleFlexDirection> direction;
        };
    }
}