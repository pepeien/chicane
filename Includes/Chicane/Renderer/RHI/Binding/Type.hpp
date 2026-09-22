#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class BindingType : std::uint8_t
            {
                UniformBuffer,
                StorageBuffer,
                SampledImage
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::BindingType inValue);
}
