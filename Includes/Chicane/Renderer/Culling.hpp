#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Culling/FrontFace.hpp"
#include "Chicane/Renderer/Culling/Mode.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Culling
        {
        public:
            CullingFrontFace frontFace = CullingFrontFace::Clockwise;
            CullingMode      mode      = CullingMode::None;
        };
    }
}