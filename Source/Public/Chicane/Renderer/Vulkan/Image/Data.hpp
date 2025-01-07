#pragma once

#include "Chicane/Core/Image.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
	    	struct Data : public Chicane::Image::Data
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