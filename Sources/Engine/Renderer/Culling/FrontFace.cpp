#include "Chicane/Renderer/Culling/FrontFace.hpp"

namespace Chicane
{
    String toString(Renderer::CullingFrontFace inValue)
    {
        switch (inValue)
        {
        case Renderer::CullingFrontFace::CounterClockwise:
            return "CounterClockwise";

        case Renderer::CullingFrontFace::Clockwise:
            return "Clockwise";

        default:
            return "";
        }
    }
}
