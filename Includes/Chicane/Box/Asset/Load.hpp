#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Box::AssetLoad inValue)
    {
        switch (inValue)
        {
        case Box::AssetLoad::Header:
            return "Header";

        case Box::AssetLoad::Preview:
            return "Preview";

        case Box::AssetLoad::Full:
            return "Full";

        default:
            return "";
        }
    }
}
