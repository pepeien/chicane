#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        class DockPanel;

        struct CHICANE_GRID DockResize
        {
        public:
            DockPanel* panel       = nullptr;
            Vec2       cursor      = Vec2::Zero();
            float      startExtent = 0.0f;
        };
    }
}
