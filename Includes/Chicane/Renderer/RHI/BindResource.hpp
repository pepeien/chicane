#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Binding/Type.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Sampler.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER BindResource
            {
            public:
                std::uint32_t binding = 0;
                BindingType   type    = BindingType::UniformBuffer;
                Buffer        buffer;
                Image         image;
                Sampler       sampler;
                std::uint32_t arrayIndex = 0;
            };
        }
    }
}
