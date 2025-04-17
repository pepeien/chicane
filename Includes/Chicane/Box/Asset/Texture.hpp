#pragma once

#include "Box/Asset/Texture/Instance.hpp"
#include "Box/Asset/Texture/Manager.hpp"
#include "Box/Asset/Texture/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            static constexpr const char*   EXTENSION = ".btex";
            static constexpr const char*   TAG       = "Texture";
            static constexpr std::uint32_t MAX_COUNT = 1000;
        }
    }
}