#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Surface
        {
            CHICANE_RUNTIME void init(
                vk::SurfaceKHR& outSurface,
                const vk::Instance& inInstance,
                void* inWindow
            );
        }
    }
}