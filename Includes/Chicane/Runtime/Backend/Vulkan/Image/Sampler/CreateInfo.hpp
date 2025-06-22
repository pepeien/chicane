#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

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