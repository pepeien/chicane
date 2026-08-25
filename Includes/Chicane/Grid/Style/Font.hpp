#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFont
        {
        public:
            using FamilyParser = StyleProperty<String>::Parser;
            using SizeParser   = StyleProperty<float>::Parser;
            using WeightParser = StyleProperty<float>::Parser;

        public:
            StyleFont();

        public:
            void refresh();

            void setProperties(const StyleRuleset::Properties& inProperties);

            void parseWith(FamilyParser inFamilyParser, SizeParser inSizeParser, WeightParser inWeightParser);

        public:
            StyleProperty<String> family;
            StyleProperty<float>  size;
            StyleProperty<float>  weight;
        };
    }
}