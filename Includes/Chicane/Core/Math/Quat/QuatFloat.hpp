#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Quat.hpp"

namespace Chicane
{
    struct CHICANE_CORE QuatFloat : public Quat<float>
    {
    public:
        template<typename... A>
        constexpr QuatFloat(A ...args)
            : Quat<float>(args...)
        {}
    };
}