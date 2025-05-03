#pragma once

#include "Core.hpp"
#include "Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            typedef std::string                            RawSide;
            typedef std::unordered_map<Side, RawSide>      RawSides;
            typedef Image::CompiledData                    CompiledSide;
            typedef std::unordered_map<Side, CompiledSide> CompiledSides;
        }
    }
}