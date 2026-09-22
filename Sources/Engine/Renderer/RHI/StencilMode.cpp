#include "Chicane/Renderer/RHI/StencilMode.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::StencilMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::StencilMode::None:
            return "None";

        case Renderer::RHI::StencilMode::WriteReplace:
            return "WriteReplace";

        case Renderer::RHI::StencilMode::TestNotEqual:
            return "TestNotEqual";

        default:
            return "";
        }
    }
}
