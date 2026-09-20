#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Sampler/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHISamplerData
        {
        public:
            RHI::SamplerCreateInfo createInfo;
        };
    }
}
