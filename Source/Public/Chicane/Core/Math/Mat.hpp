#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Mat : glm::mat<O, O, T, Q>
    {
        template<typename... A>
        Mat(A ...args)
            : glm::mat<O, O, T, Q>(args...)
        {}
    };
}