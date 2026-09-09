#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class TextureMap : std::uint8_t
    {
        Base = 0,
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

    static constexpr inline std::uint8_t TEXTURE_MAP_COUNT = static_cast<std::uint8_t>(TextureMap::Count);

    CHICANE_CORE String toString(TextureMap inValue);
    CHICANE_CORE TextureMap toTextureMap(const String& inValue);
}
