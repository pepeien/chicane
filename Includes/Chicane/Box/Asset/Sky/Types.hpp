#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyRawSide  = String;
        using SkyRawSides = std::unordered_map<SkySide, SkyRawSide>;

        using SkyParsedSide  = Image::Instance;
        using SkyParsedSides = std::unordered_map<SkySide, SkyParsedSide>;
    }
}