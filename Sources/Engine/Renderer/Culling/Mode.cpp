#include "Chicane/Renderer/Culling/Mode.hpp"

namespace Chicane
{
    String toString(Renderer::CullingMode inValue)
    {
        switch (inValue)
        {
        case Renderer::CullingMode::None:
            return "None";

        case Renderer::CullingMode::Front:
            return "Front";

        case Renderer::CullingMode::Back:
            return "Back";

        case Renderer::CullingMode::FrontAndBack:
            return "FrontAndBack";

        default:
            return "";
        }
    }
}
