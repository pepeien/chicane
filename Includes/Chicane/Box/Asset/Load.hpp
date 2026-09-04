#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Box
    {
        enum class AssetLoad : std::uint8_t
        {
            Header,
            Preview,
            Full
        };
    }
}
