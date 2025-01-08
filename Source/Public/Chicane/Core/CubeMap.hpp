#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        enum class Side : std::uint8_t
        {
            Left,
            Right,
            Front,
            Back,
            Up,
            Down
        };

        typedef std::unordered_map<Side, Image::Instance> Instance;
        typedef std::unordered_map<Side, Image::Data> Data;
    }
}