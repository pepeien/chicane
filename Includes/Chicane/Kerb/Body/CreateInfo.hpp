#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body/Shape.hpp"
#include "Chicane/Kerb/Collision/Group.hpp"
#include "Chicane/Kerb/Collision/Preset.hpp"
#include "Chicane/Kerb/Gravity.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

namespace Chicane
{
    namespace Kerb
    {
        using BodyPolygon = std::pair<Vertex::Indices, Vertex::List>;

        CH_TYPE(Manual)
        struct CHICANE_KERB BodyCreateInfo
        {
        public:
            CH_FIELD()
            BodyShape shape = BodyShape::Box;

            CH_FIELD()
            MotionType motion = MotionType::Static;

            CH_FIELD()
            float mass = 30.0f;

            CH_FIELD()
            float massScale = 1.0f;

            CH_FIELD()
            float gravityFactor = Gravity::FactorFull;

            CH_FIELD()
            ObjectLayer layer = ObjectLayer::Auto;

            CH_FIELD()
            CollisionPreset preset = CollisionPreset::Custom;

            CH_FIELD()
            bool bIsSensor = false;

        public:
            CollisionGroup group = {};

            Bounds3D       bounds = {};

            BodyPolygon    polygon = {}; // Only used if `shape` is `BodyShape::Polygon`
        };
    }
}
