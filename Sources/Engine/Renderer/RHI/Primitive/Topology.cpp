#include "Chicane/Renderer/RHI/Primitive/Topology.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::PrimitiveTopology inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::PrimitiveTopology::TriangleList:
            return "TriangleList";

        case Renderer::RHI::PrimitiveTopology::TriangleStrip:
            return "TriangleStrip";

        case Renderer::RHI::PrimitiveTopology::LineList:
            return "LineList";

        default:
            return "";
        }
    }
}
