#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body/Shape.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    namespace Kerb
    {
        using BodyPolygon = std::pair<Vertex::Indices, Vertex::List>;

        struct CHICANE_KERB BodyCreateInfo
        {
        public:
            BodyShape   shape   = BodyShape::Box;
            MotionType  motion  = MotionType::Static;
            Bounds3D    bounds  = {};
            float       mass    = 30.0f;
            BodyPolygon polygon = {}; //  Only used if `shape` is `BodyShape::Polygon`
        };
    }
}