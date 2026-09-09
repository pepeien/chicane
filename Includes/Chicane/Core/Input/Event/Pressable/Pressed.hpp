#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Input
    {
        template <typename B>
        struct PressablePressed
        {
            B           button;
            Time::Point lastFire;
        };
    }
}
