#include "Chicane/Renderer/RHI/Image/Format.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::ImageFormat inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::ImageFormat::RGBA8:
            return "RGBA8";

        case Renderer::RHI::ImageFormat::BGRA8:
            return "BGRA8";

        case Renderer::RHI::ImageFormat::RGBA16F:
            return "RGBA16F";

        case Renderer::RHI::ImageFormat::Depth32F:
            return "Depth32F";

        case Renderer::RHI::ImageFormat::Depth24Stencil8:
            return "Depth24Stencil8";

        default:
            return "";
        }
    }
}
