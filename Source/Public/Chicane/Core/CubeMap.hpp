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

        static constexpr std::uint32_t          IMAGE_COUNT = 6;
        static const std::vector<CubeMap::Side> IMAGE_ORDER = {
            CubeMap::Side::Left,
            CubeMap::Side::Right,
            CubeMap::Side::Front,
            CubeMap::Side::Back,
            CubeMap::Side::Up,
            CubeMap::Side::Down
        };
    }
}