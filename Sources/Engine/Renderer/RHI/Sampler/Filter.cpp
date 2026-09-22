#include "Chicane/Renderer/RHI/Sampler/Filter.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::SamplerFilter inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::SamplerFilter::Nearest:
            return "Nearest";

        case Renderer::RHI::SamplerFilter::Linear:
            return "Linear";

        default:
            return "";
        }
    }
}
