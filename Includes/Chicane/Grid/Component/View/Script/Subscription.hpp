#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID ViewScriptSubscription
        {
        public:
            std::uint64_t token = 0;
            int           ref   = 0;
        };
    }
}
