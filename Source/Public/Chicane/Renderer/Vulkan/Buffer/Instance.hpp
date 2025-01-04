#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
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
}