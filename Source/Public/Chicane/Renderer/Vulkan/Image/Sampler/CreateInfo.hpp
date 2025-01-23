#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Sampler
            {
                struct CreateInfo
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