#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input/Device/Types.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE DeviceEvent
        {
        public:
            DeviceID device = std::numeric_limits<std::uint32_t>::max();
        };
    }
}