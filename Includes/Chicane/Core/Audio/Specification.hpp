#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    struct CHICANE_CORE AudioSpecification
    {
    public:
        int format    = 0;
        int channels  = 0;
        int frequency = 0;
    };
}