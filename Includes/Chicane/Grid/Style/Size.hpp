#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSize
        {
        public:
            using Parser = StyleProperty<float>::Parser;

        public:
            StyleSize();

        public:
            void refresh();

            void setProperties(
                const StyleRuleset::Properties& inProperties,
                const String&                   inValueName,
                const String&                   inMinName,
                const String&                   inMaxName
            );

            void parseWith(Parser inValueParser, Parser inLimitParser);
            void copyValue(const StyleSize& inOther);

            bool isAuto() const;
            bool hasMin() const;
            bool hasMax() const;
            void clamp(float& outValue) const;

        public:
            StyleProperty<float> value;
            StyleProperty<float> min;
            StyleProperty<float> max;
        };
    }
}
