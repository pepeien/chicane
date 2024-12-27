#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct Quat : glm::qua<T, Q>
    {
        template<typename... A>
        Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };
}