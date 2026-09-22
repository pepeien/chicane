#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Box.hpp"

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

    CHICANE_BOX String toString(Box::AssetLoad inValue);
}
