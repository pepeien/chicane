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

        inline String toString(AssetLoad inValue)
        {
            switch (inValue)
            {
            case AssetLoad::Header:
                return "Header";

            case AssetLoad::Preview:
                return "Preview";

            case AssetLoad::Full:
                return "Full";

            default:
                return "";
            }
        }
    }
}
