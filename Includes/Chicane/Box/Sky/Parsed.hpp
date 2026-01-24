#pragma once

#include <unordered_map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Sky/Side.hpp"

#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyParsedSide  = Image;
        using SkyParsedSides = std::unordered_map<SkySide, SkyParsedSide>;
    }
}