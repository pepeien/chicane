#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Image/Vendor.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Texture/Map.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace TextureGltf
        {
            struct CHICANE_BOX Entry
            {
                String      id;
                ImageVendor vendor = ImageVendor::Undefined;
                Image::Raw  data   = {};
            };

            struct CHICANE_BOX Parsed
            {
                std::vector<Entry>                                                   images     = {};
                std::unordered_map<std::int32_t, std::map<TextureMap, std::int32_t>> materials = {};
            };

            CHICANE_BOX Parsed parse(const FileSystem::Path& inFilepath);
        }
    }
}
