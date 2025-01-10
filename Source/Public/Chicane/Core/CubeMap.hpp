#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/CubeMap/Side.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        typedef std::unordered_map<Side, Image::Instance> Instance;
        typedef std::unordered_map<Side, Image::Data>     Data;

        static constexpr std::uint32_t IMAGE_COUNT = 6;
        static const std::vector<Side> IMAGE_ORDER = {
            Side::Left,
            Side::Right,
            Side::Front,
            Side::Back,
            Side::Up,
            Side::Down
        };
    }
}