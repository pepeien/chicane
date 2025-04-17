#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Sampler
            {
                struct CHICANE CreateInfo
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