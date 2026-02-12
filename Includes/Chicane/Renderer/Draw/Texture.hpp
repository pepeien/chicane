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
            static const DrawTexture& empty()
            {
                static const DrawTexture result;

                return result;
            }

        public:
            Image image = {};
        };
    }
}