#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Binding.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER BindGroupLayoutCreateInfo
            {
            public:
                std::vector<Binding> bindings = {};
            };
        }
    }
}
