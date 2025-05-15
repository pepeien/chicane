#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    template<std::uint32_t O>
    struct CHICANE_CORE Curve : public Line
    {
    public:
        std::array<Vec<2, float>, O> controls = {};
    };
}