#include "Chicane/Renderer/Draw/Sky/Kind.hpp"

namespace Chicane
{
    String toString(Renderer::DrawSkyKind inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawSkyKind::Cube:
            return "Cube";

        case Renderer::DrawSkyKind::Panorama:
            return "Panorama";

        default:
            return "";
        }
    }
}
