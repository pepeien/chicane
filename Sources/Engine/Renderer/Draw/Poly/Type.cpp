#include "Chicane/Renderer/Draw/Poly/Type.hpp"

namespace Chicane
{
    String toString(Renderer::DrawPolyType inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPolyType::e2D:
            return "e2D";

        case Renderer::DrawPolyType::e3D:
            return "e3D";

        case Renderer::DrawPolyType::eParticle:
            return "eParticle";

        default:
            return "";
        }
    }
}
