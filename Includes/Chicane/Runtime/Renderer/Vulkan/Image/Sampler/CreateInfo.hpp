#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Sampler
            {
                struct CHICANE_RUNTIME CreateInfo
                {
                public:
                    vk::SamplerAddressMode addressMode;
                    vk::BorderColor        borderColor;

                    vk::Device             logicalDevice;
                };
            }
        }
    }
}