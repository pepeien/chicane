#pragma once

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawTextureData : DrawData
        {
        public:
            String reference = "";
            Image  image     = {};
        };
    }
}