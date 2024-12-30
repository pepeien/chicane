#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Camera
    {
        struct Settings
        {
        public:
            Vec<2, std::uint32_t> viewport    = Vec<2, std::uint32_t>(0);
            float                 aspectRatio = 0.0f;
            float                 fieldOfView = 45.0f;
            float                 nearClip    = 0.1f;
            float                 farClip     = 5000.0f;
        };
    }
}