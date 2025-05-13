#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace View
    {
        struct CHICANE Settings
        {
        public:
            Vec<2, std::uint32_t> viewport    = Vec<2, std::uint32_t>(0);
            float                 aspectRatio = 0.0f;
            float                 fieldOfView = 45.0f;
            float                 nearClip    = 0.1f;
            float                 farClip     = 1000.0f;
        };
    }
}