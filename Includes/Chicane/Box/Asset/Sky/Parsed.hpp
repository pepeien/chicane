#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyParsedSide  = Image;
        using SkyParsedSides = std::unordered_map<SkySide, SkyParsedSide>;
    }
}