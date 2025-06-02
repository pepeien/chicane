#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            struct CHICANE_RUNTIME Instance
            {
            public:
                vk::Buffer       instance;
                vk::DeviceMemory memory;
            };
        }
    }
}