#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Texture/Map.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DInstance
        {
        public:
            using List = std::vector<DrawPoly3DInstance>;
            using Map  = std::map<Draw::Id, List>;

        public:
            Mat4     model                       = Mat4::One;
            Draw::Id textures[TEXTURE_MAP_COUNT] = {
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId,
                Draw::InvalidId
            };
        };

        static_assert(
            sizeof(DrawPoly3DInstance) == 64 + (sizeof(Draw::Id) * TEXTURE_MAP_COUNT),
            "Poly3D instance texture slots must match TextureMap::Count"
        );
    }
}
