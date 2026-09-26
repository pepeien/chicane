#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Script
    {
        struct CHICANE_CORE Event
        {
        public:
            String name = String::sEmpty();
            String data = String::sEmpty();
        };
    }
}
