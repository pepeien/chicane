#pragma once

#include "Chicane/Drift/Tween.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property/Id.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleAnimatorTween
        {
        public:
            StylePropertyId id    = StylePropertyId::Count;
            Drift::Tween    tween = {};
        };
    }
}
