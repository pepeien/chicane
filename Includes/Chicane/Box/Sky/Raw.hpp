#pragma once

#include <unordered_map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Sky/Side.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyRawSide  = String;
        using SkyRawSides = std::unordered_map<SkySide, SkyRawSide>;
    }
}