#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/CubeMap/Side.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace CubeMap
        {
            typedef std::unordered_map<Side, Image::Instance> RawData;
            typedef std::unordered_map<Side, Image::Data>     CompiledData;
        }
    }
}