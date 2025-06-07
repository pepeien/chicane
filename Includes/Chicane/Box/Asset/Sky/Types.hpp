#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            using RawSide  = std::string;
            using RawSides = std::unordered_map<Side, RawSide>;

            using ParsedSide  = Image::Instance;
            using ParsedSides = std::unordered_map<Side, ParsedSide>;
        }
    }
}