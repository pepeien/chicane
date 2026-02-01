#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/View/Projection/Type.hpp"

namespace Chicane
{
    struct CHICANE_CORE ViewSettings
    {
    public:
        Vec<2, std::uint32_t> viewport    = Vec<2, std::uint32_t>(0);
        float                 aspectRatio = 0.0f;
        float                 fieldOfView = 45.0f;
        float                 nearClip    = 0.1f;
        float                 farClip     = 1000.0f;
        ViewProjectionType    projection  = ViewProjectionType::Perspective;
    };
}