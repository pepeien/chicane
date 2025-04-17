#pragma once

#include "Box/Asset/Sky/Instance.hpp"
#include "Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            static constexpr const char* EXTENSION = ".bsky";
            static constexpr const char* TAG       = "Sky";

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