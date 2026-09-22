#include "Chicane/Renderer/Draw/Poly/Mode.hpp"

namespace Chicane
{
    String toString(Renderer::DrawPolyMode inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPolyMode::Fill:
            return "Fill";

        case Renderer::DrawPolyMode::Line:
            return "Line";

        default:
            return "";
        }
    }
}
