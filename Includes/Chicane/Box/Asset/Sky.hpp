#pragma once

#include "Chicane/Box/Asset/Sky/Instance.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            static inline constexpr const char* EXTENSION = ".bsky";
            static inline constexpr const char* TAG       = "Sky";

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