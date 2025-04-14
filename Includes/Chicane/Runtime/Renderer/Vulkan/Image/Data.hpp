#pragma once

#include "Chicane/Core/Image.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
	    	struct Data : public Chicane::Image::CompiledData
	    	{
            public:
	    		vk::Image        instance;
	    		vk::ImageView    view;
	    		vk::DeviceMemory memory;
	    		vk::Sampler      sampler;
                vk::Format       format;
                vk::Extent2D     extent;
	    	};
        }
    }
}