#pragma once

#include <vector>

#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        struct CHICANE_GRID ComponentPaintContext
        {
        public:
            Mat3                           world      = Mat3(1.0f);
            Bounds2D                       clip       = Bounds2D::unconstrained();
            Vec2                           scroll     = Vec2::Zero();
            float                          opacity    = 1.0f;
            float                          filterBlur = 0.0f;
            float                          depth      = 0.0f;

            std::vector<const Component*>* roundedAncestors = nullptr;
        };
    }
}
