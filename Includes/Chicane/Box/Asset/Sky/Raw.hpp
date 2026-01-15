#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyRawSide  = String;
        using SkyRawSides = std::unordered_map<SkySide, SkyRawSide>;
    }
}