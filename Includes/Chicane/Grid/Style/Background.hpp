#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleBackground
        {
        public:
            String      image = "";
            Color::Rgba color = Color::toRgba(Color::TEXT_COLOR_TRANSPARENT);
        };
    }
}