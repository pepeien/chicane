#pragma once

#include <unordered_map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Reference.hpp"
#include "Chicane/Box/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        using SkyRawSides = std::unordered_map<SkySide, AssetReference>;
    }
}