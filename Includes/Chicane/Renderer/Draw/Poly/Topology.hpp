#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPolyTopology : std::uint8_t
        {
            PointList,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip,
            TriangleFan,
            LineListWithAdjacency,
            LineStripWithAdjacency,
            TriangleListWithAdjacency,
            TriangleStripWithAdjacency,
            PatchList
        };

        inline String toString(DrawPolyTopology inValue)
        {
            switch (inValue)
            {
            case DrawPolyTopology::PointList:
                return "PointList";

            case DrawPolyTopology::LineList:
                return "LineList";

            case DrawPolyTopology::LineStrip:
                return "LineStrip";

            case DrawPolyTopology::TriangleList:
                return "TriangleList";

            case DrawPolyTopology::TriangleStrip:
                return "TriangleStrip";

            case DrawPolyTopology::TriangleFan:
                return "TriangleFan";

            case DrawPolyTopology::LineListWithAdjacency:
                return "LineListWithAdjacency";

            case DrawPolyTopology::LineStripWithAdjacency:
                return "LineStripWithAdjacency";

            case DrawPolyTopology::TriangleListWithAdjacency:
                return "TriangleListWithAdjacency";

            case DrawPolyTopology::TriangleStripWithAdjacency:
                return "TriangleStripWithAdjacency";

            case DrawPolyTopology::PatchList:
                return "PatchList";

            default:
                return "";
            }
        }
    }
}