#pragma once

#include <array>

namespace Editor
{
    struct GridPush
    {
    public:
        std::array<float, 4> color = {};
        std::array<float, 4> cells = {};
        std::array<float, 4> extra = {};
    };
}
