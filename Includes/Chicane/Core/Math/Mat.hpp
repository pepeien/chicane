#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#include "Chicane/Core.hpp"

namespace Chicane
{
    template <std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct CHICANE_CORE Mat : glm::mat<O, O, T, Q>
    {
    public:
        template <typename... A>
        constexpr Mat(A... args)
            : glm::mat<O, O, T, Q>(args...)
        {}
    };
}