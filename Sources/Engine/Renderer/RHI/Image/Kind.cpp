#include "Chicane/Renderer/RHI/Image/Kind.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::ImageKind inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::ImageKind::Color2D:
            return "Color2D";

        case Renderer::RHI::ImageKind::Depth2D:
            return "Depth2D";

        case Renderer::RHI::ImageKind::Depth2DArray:
            return "Depth2DArray";

        case Renderer::RHI::ImageKind::Cube:
            return "Cube";

        default:
            return "";
        }
    }
}
