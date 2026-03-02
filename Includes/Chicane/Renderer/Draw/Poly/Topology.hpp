#pragma once

#include <cstdint>

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
    }
}