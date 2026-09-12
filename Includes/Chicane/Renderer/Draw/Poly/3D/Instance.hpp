#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Texture/Map.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"

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
            bool has(DrawPoly3DFlag inFlag) const { return Renderer::has(flags, inFlag); }

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
            DrawPoly3DFlag flags            = DrawPoly3DFlag::Lit | DrawPoly3DFlag::Shadow;
            float          emissiveStrength = 1.0f;
            std::int32_t   _pad1            = 0;
            std::int32_t   _pad2            = 0;
        };
    }
}
