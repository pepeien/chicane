#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleBackground
        {
        public:
            using ColorParser = StyleProperty<Color::Rgba>::Parser;
            using ImageParser = StyleProperty<String>::Parser;

        public:
            StyleBackground();

        public:
            void refresh();

            void setProperties(const StyleRuleset::Properties& inProperties);

            void parseWith(ColorParser inColorParser, ImageParser inImageParser);

        public:
            StyleProperty<Color::Rgba> color;
            StyleProperty<String>      image;
        };
    }
}