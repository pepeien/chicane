#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Reference.hpp"

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