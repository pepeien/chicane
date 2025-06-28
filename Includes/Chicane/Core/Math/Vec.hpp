#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct CHICANE_CORE Vec : glm::vec<O, T, Q>
    {
    public:
        template<typename... A>
        constexpr Vec(A ...args)
            : glm::vec<O, T, Q>(args...)
        {}

    public:
        Vec<O, T, Q> normalize() const
        {
            return glm::normalize<O, T, Q>(*this);
        }

        T dot(const Vec<O, T, Q>& inValue) const
        {
            return glm::dot<O, T, Q>(*this, inValue);
        }
    };
}