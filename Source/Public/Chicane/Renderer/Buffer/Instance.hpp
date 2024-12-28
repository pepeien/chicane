#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Buffer
    {
        struct Instance
        {
        public:
            vk::Buffer       instance;
            vk::DeviceMemory memory;
        };
    }
}