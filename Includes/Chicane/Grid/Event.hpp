#pragma once

#include "Chicane/Core/Reference.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Event
        {
        public:
            std::vector<Reference> values = {};
        };
    }
}