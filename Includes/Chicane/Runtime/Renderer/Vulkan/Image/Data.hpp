#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
	    	struct CHICANE_RUNTIME Data : public Chicane::Image::Instance
	    	{
            public:
                Data(const Chicane::Image::Instance& inImage)
                    : Super()
                {
                    m_width   = inImage.getWidth();
                    m_height  = inImage.getWidth();
                    m_channel = inImage.getChannel();
                    m_format  = inImage.getFormat();
                    m_pixels  = inImage.getPixels();
                }

                Data()
                    : Super()
                {}

                ~Data() = default;

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