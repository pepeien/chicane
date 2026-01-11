#pragma once

#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        namespace BroadPhaseLayer
        {
            static constexpr std::uint16_t NonMoving = 0;
            static constexpr std::uint16_t Moving    = 1;
            static constexpr std::uint16_t Count     = 2;
        }
    }
}