#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
	    	struct Bundle
	    	{
            public:
	    		vk::Image        instance;
	    		vk::DeviceMemory memory;
	    		vk::ImageView    view;
	    		vk::Sampler      sampler;
	    	};
        }
    }
}