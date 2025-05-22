#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct CHICANE_CORE Quat : glm::qua<T, Q>
    {
        template<typename... A>
        constexpr Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };
}