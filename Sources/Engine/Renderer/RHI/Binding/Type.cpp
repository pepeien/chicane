#include "Chicane/Renderer/RHI/Binding/Type.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::BindingType inValue)
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
