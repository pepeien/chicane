#pragma once

#include "Chicane/Core/Color.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Sampler/Address.hpp"
#include "Chicane/Renderer/RHI/Sampler/Filter.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER SamplerCreateInfo
            {
            public:
                SamplerFilter  minFilter   = SamplerFilter::Linear;
                SamplerFilter  magFilter   = SamplerFilter::Linear;
                SamplerAddress address     = SamplerAddress::ClampToEdge;
                bool           bHasMip     = false;
                Color::Rgba    borderColor = {};
            };
        }
    }
}
