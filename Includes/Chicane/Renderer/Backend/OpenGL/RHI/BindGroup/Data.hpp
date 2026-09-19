#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindResource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHIGroupData
        {
        public:
            std::vector<RHI::BindResource> resources;
        };
    }
}
