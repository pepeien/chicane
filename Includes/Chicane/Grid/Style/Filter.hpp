#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFilter
        {
        public:
            using BlurParser = StyleProperty<float>::Parser;

        public:
            StyleFilter();

        public:
            void refresh();

            void setProperties(const StyleRuleset::Properties& inProperties, const String& inAttributeName);

            void parseWith(BlurParser inBlurParser);

        public:
            StyleProperty<float> blur;
        };
    }
}
