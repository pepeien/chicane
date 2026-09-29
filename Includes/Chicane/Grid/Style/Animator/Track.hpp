#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property/Id.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleAnimatorTrack
        {
        public:
            StylePropertyId id   = StylePropertyId::Count;
            String          name = String::sEmpty();
        };
    }
}
