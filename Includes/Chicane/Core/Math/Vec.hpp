#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct CHICANE Vec : glm::vec<O, T, Q>
    {
        template<typename... A>
        Vec(A ...args)
            : glm::vec<O, T, Q>(args...)
        {}

        T dot(const Vec<O, T, Q>& inTarget) const
        {
            return glm::dot<O, T, Q>(*this, inTarget);
        }
    };

    const Vec<2, float> Vec2Zero = Vec<2, float>(0.0f);
    const Vec<3, float> Vec3Zero = Vec<3, float>(0.0f);
}