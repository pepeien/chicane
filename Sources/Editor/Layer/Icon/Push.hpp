#pragma once

#include <array>
#include <cstdint>

namespace Editor
{
    struct IconPush
    {
    public:
        std::array<float, 4>        positionSize = {};
        std::array<float, 4>        color        = {1.0f, 1.0f, 1.0f, 1.0f};
        std::array<std::int32_t, 4> extra        = {};
    };
}
