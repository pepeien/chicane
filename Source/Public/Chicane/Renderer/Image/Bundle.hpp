#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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