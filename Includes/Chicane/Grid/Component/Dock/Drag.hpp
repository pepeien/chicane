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
            DockPanel* panel     = nullptr;
            Vec2       cursor    = Vec2::sZero();
            Vec2       grab      = Vec2::sZero();
            bool       bIsActive = false;
            DockSide   drop      = DockSide::Float;
        };
    }
}
