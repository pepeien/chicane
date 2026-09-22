#include "Chicane/Renderer/Depth/Compare.hpp"

namespace Chicane
{
    String toString(Renderer::DepthCompare inValue)
    {
        switch (inValue)
        {
        case Renderer::DepthCompare::Never:
            return "Never";

        case Renderer::DepthCompare::Less:
            return "Less";

        case Renderer::DepthCompare::Equal:
            return "Equal";

        case Renderer::DepthCompare::LessOrEqual:
            return "LessOrEqual";

        case Renderer::DepthCompare::Greater:
            return "Greater";

        case Renderer::DepthCompare::NotEqual:
            return "NotEqual";

        case Renderer::DepthCompare::GreaterOrEqual:
            return "GreaterOrEqual";

        case Renderer::DepthCompare::Always:
            return "Always";

        default:
            return "";
        }
    }
}
