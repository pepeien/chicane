#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Renderer::RHI::BindingType inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::BindingType::UniformBuffer:
            return "UniformBuffer";

        case Renderer::RHI::BindingType::StorageBuffer:
            return "StorageBuffer";

        case Renderer::RHI::BindingType::SampledImage:
            return "SampledImage";

        default:
            return "";
        }
    }
}
