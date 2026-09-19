#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static constexpr inline const std::uint32_t RHI_SET_FRAME    = 0;
        static constexpr inline const std::uint32_t RHI_SET_TEXTURES = 1;

        static constexpr inline const std::uint32_t RHI_BINDING_CAMERA         = 0;
        static constexpr inline const std::uint32_t RHI_BINDING_LIGHT          = 1;
        static constexpr inline const std::uint32_t RHI_BINDING_MESH_INSTANCES = 2;
        static constexpr inline const std::uint32_t RHI_BINDING_SHADOW_MAP     = 3;
        static constexpr inline const std::uint32_t RHI_BINDING_SKY_MAP        = 4;
        static constexpr inline const std::uint32_t RHI_BINDING_PARTICLES      = 1;
        static constexpr inline const std::uint32_t RHI_BINDING_UI_INSTANCES   = 0;
        static constexpr inline const std::uint32_t RHI_BINDING_UI_GLYPHS      = 1;
        static constexpr inline const std::uint32_t RHI_BINDING_UI_BACKDROP    = 2;
        static constexpr inline const std::uint32_t RHI_BINDING_UI_SCREEN      = 3;
        static constexpr inline const std::uint32_t RHI_BINDING_POST_SOURCE    = 0;
        static constexpr inline const std::uint32_t RHI_BINDING_POST_BLOOM     = 1;
        static constexpr inline const std::uint32_t RHI_BINDING_TEXTURES       = 16;
        static constexpr inline const std::uint32_t RHI_BINDING_PUSH_CONSTANTS = 15;
    }
}
