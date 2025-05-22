#pragma once

#include "Chicane/Grid/Essential.hpp"

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