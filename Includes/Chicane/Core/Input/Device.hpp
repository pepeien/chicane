#pragma once

#include <cstdint>
#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Type.hpp"

namespace Chicane
{
    namespace Input
    {
        using DeviceID  = std::uint32_t;
        using DeviceMap = std::unordered_map<DeviceType, DeviceID>;
    }
}