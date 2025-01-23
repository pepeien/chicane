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
	    		vk::ImageView    view;
	    		vk::DeviceMemory memory;
	    		vk::Sampler      sampler;
                vk::Format       format;
	    	};
        }
    }
}