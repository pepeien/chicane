#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class TextureMaterial : std::uint8_t
    {
        Albedo = 0,
        AmbientOcclusion,
        Normal,
        Roughness,
        Metalness,
        Height,
        Specular,
        Opacity,
        Refraction,
        SelfIllumination,
        Bump,
        Reflection,

        Count
    };

    static constexpr inline std::uint8_t TEXTURE_MATERIAL_COUNT = static_cast<std::uint8_t>(TextureMaterial::Count);

    CHICANE_CORE String toString(TextureMaterial inValue);
    CHICANE_CORE TextureMaterial toTextureMaterial(const String& inValue);
}
