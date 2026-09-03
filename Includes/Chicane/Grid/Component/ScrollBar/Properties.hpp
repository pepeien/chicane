#pragma once

#include "Chicane/Core/Math/Bounds/2D.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/ScrollBar.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID ScrollBarProperties
        {
        public:
            inline void destroy()
            {
                delete trackBar;
                delete thumbBar;

                trackBar = nullptr;
                thumbBar = nullptr;
            }

            inline void append(std::vector<Component*>& outComponents) const
            {
                if (trackBar)
                {
                    outComponents.push_back(trackBar);
                }

                if (thumbBar)
                {
                    outComponents.push_back(thumbBar);
                }
            }

        public:
            bool       bIsVisible  = false;
            bool       bIsDragging = false;
            float      grab        = 0.0f;

            Bounds2D   track       = {};
            Bounds2D   thumb       = {};

            ScrollBar* trackBar    = nullptr;
            ScrollBar* thumbBar    = nullptr;
        };
    }
}
