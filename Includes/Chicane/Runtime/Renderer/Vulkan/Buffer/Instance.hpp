#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            struct CHICANE Instance
            {
            public:
                vk::Buffer       instance;
                vk::DeviceMemory memory;
            };
        }
    }
}