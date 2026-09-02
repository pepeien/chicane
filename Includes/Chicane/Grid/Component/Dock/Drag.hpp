#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Dock/Side.hpp"

namespace Chicane
{
    namespace Grid
    {
        class DockPanel;

        struct CHICANE_GRID DockDrag
        {
        public:
            DockPanel* panel   = nullptr;
            Vec2       cursor  = Vec2::Zero();
            Vec2       grab    = Vec2::Zero();
            bool       bActive = false;
            DockSide   drop    = DockSide::Float;
        };
    }
}
