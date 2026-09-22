#include "Chicane/Renderer/RHI/Sampler/Address.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::SamplerAddress inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::SamplerAddress::ClampToEdge:
            return "ClampToEdge";

        case Renderer::RHI::SamplerAddress::ClampToBorder:
            return "ClampToBorder";

        case Renderer::RHI::SamplerAddress::Repeat:
            return "Repeat";

        default:
            return "";
        }
    }
}
