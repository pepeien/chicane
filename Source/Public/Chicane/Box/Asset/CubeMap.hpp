#pragma once

#include "Chicane/Box/Asset/CubeMap/Data.hpp"
#include "Chicane/Box/Asset/CubeMap/Instance.hpp"
#include "Chicane/Box/Asset/CubeMap/Manager.hpp"
#include "Chicane/Box/Asset/CubeMap/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace CubeMap
        {
            static constexpr const char* EXTENSION = ".bcmp";
            static constexpr const char* TAG       = "CubeMap";

            static const std::vector<Side> ORDER = {
                Side::Left,
                Side::Right,
                Side::Front,
                Side::Back,
                Side::Up,
                Side::Down
            };
        }
    }
}