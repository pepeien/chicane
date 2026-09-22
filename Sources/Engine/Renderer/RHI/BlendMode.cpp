#include "Chicane/Renderer/RHI/BlendMode.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::BlendMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::BlendMode::None:
            return "None";

        case Renderer::RHI::BlendMode::Alpha:
            return "Alpha";

        case Renderer::RHI::BlendMode::Additive:
            return "Additive";

        default:
            return "";
        }
    }
}
