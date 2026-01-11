#pragma once

#include "Chicane/Core.hpp"

#include <glm/gtc/matrix_transform.hpp>

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