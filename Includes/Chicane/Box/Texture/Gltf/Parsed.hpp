#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Texture/Gltf/Entry.hpp"
#include "Chicane/Core/Texture/Map.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace TextureGltf
        {
            struct CHICANE_BOX Parsed
            {
                std::vector<Entry>                                                   images             = {};
                std::unordered_map<std::int32_t, std::map<TextureMap, std::int32_t>> materials          = {};
                std::unordered_map<std::int32_t, float>                              emissiveStrengths = {};
            };
        }
    }
}
