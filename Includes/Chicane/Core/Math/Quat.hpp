#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Chicane/Core.hpp"

namespace Chicane
{
    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct CHICANE_CORE Quat : glm::qua<T, Q>
    {
    public:
        template<typename... A>
        constexpr Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };
}