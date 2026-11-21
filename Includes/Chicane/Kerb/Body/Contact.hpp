#pragma once

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body.hpp"

namespace Chicane
{
    namespace Kerb
    {
        struct CHICANE_KERB BodyContact
        {
        public:
            Body first  = {};
            Body second = {};
        };
    }
}
