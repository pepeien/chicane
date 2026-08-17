#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        // Outline of a single glyph, expressed as quadratic Bezier segments in em units.
        // The renderer uploads these to the GPU so the fragment shader can resolve coverage analytically.
        struct CHICANE_RENDERER DrawGlyphData : public DrawData
        {
        public:
            // Three points per segment, laid out as start, control, end
            using Points = std::vector<Vec2>;

        public:
            Vec2   boundsMin = Vec2::Zero();
            Vec2   boundsMax = Vec2::Zero();
            Points points    = {};
        };
    }
}
