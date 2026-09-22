#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"

namespace Chicane
{
    String toString(Renderer::DrawPoly3DFlag inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPoly3DFlag::None:
            return "None";

        case Renderer::DrawPoly3DFlag::Lit:
            return "Lit";

        case Renderer::DrawPoly3DFlag::Shadow:
            return "Shadow";

        case Renderer::DrawPoly3DFlag::Foreground:
            return "Foreground";

        case Renderer::DrawPoly3DFlag::Outlined:
            return "Outlined";

        default:
            return "";
        }
    }
}
