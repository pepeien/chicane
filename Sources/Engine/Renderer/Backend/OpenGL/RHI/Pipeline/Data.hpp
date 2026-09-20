#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Pipeline/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHIPipelineData
        {
        public:
            std::uint32_t           program    = 0;
            std::uint32_t           vao        = 0;
            RHI::PipelineCreateInfo createInfo = {};
            std::uint32_t           pushUbo    = 0;
        };
    }
}
