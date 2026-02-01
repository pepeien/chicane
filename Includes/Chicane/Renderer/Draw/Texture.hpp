#pragma once

#include <vector>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawTexture : Draw
        {
        public:
            using List = std::vector<DrawTexture>;

        public:
            Image image = {};
        };
    }
}