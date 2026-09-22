#include "Chicane/Renderer/RHI/FillMode.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::FillMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::FillMode::Fill:
            return "Fill";

        case Renderer::RHI::FillMode::Line:
            return "Line";

        default:
            return "";
        }
    }
}
