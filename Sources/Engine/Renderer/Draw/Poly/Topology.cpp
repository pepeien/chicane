#include "Chicane/Renderer/Draw/Poly/Topology.hpp"

namespace Chicane
{
    String toString(Renderer::DrawPolyTopology inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPolyTopology::PointList:
            return "PointList";

        case Renderer::DrawPolyTopology::LineList:
            return "LineList";

        case Renderer::DrawPolyTopology::LineStrip:
            return "LineStrip";

        case Renderer::DrawPolyTopology::TriangleList:
            return "TriangleList";

        case Renderer::DrawPolyTopology::TriangleStrip:
            return "TriangleStrip";

        case Renderer::DrawPolyTopology::TriangleFan:
            return "TriangleFan";

        case Renderer::DrawPolyTopology::LineListWithAdjacency:
            return "LineListWithAdjacency";

        case Renderer::DrawPolyTopology::LineStripWithAdjacency:
            return "LineStripWithAdjacency";

        case Renderer::DrawPolyTopology::TriangleListWithAdjacency:
            return "TriangleListWithAdjacency";

        case Renderer::DrawPolyTopology::TriangleStripWithAdjacency:
            return "TriangleStripWithAdjacency";

        case Renderer::DrawPolyTopology::PatchList:
            return "PatchList";

        default:
            return "";
        }
    }
}
