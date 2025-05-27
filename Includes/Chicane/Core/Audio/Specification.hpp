#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Audio
    {
        struct CHICANE_CORE Specification
        {
        public:
            int format    = 0;
            int channels  = 0;
            int frequency = 0;
        };
    }
}