#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift/Easing.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleTransition
        {
        public:
            using List = std::vector<StyleTransition>;

        public:
            StyleTransition();

        public:
            String             property;
            float              duration;
            float              delay;
            Drift::EasingCurve easing;
        };

        CHICANE_GRID bool isTime(const String& inValue);
        CHICANE_GRID bool isEasing(const String& inValue);
        CHICANE_GRID float parseTime(const String& inValue);
        CHICANE_GRID Drift::EasingCurve parseEasing(const String& inValue);
        CHICANE_GRID std::vector<String> splitStyleList(const String& inValue);
    }
}
