#pragma once

#include "Chicane/Core/Color.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleGradientStop
        {
        public:
            Color::Rgba color  = Color::toRgba(Color::TEXT_COLOR_TRANSPARENT);
            float       offset = -1.0f;
        };
    }
}
