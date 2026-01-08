#pragma once

#include "Chicane/Screech/Device.hpp"
#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        CHICANE_SCREECH void* getContext();
        CHICANE_SCREECH void* getEngine();
        CHICANE_SCREECH std::vector<Device> getDevices();
    }
}