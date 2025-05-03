#pragma once

#include "Core.hpp"
#include "Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE Event
        {
        public:
            std::vector<Reference> values = {};
        };
    }
}