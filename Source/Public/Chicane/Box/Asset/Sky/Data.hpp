#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

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