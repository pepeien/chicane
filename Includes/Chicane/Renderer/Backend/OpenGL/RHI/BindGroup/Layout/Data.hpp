#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHILayoutData
        {
        public:
            RHI::BindGroupLayoutCreateInfo createInfo;
        };
    }
}
