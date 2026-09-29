#pragma once

#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "Chicane/Core.hpp"

namespace Chicane
{
    template <std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Vec : glm::vec<O, T, Q>
    {
    public:
        template <typename... A>
        constexpr Vec(A... args)
            : glm::vec<O, T, Q>(args...)
        {}

    public:
        inline Vec<O, T, Q> normalize() const
        {
            T lengthSquared = T(0);

            for (std::uint32_t index = 0; index < O; index++)
            {
                const T component = (*this)[index];

                lengthSquared += component * component;
            }

            Vec<O, T, Q> result(T(0));
            const T      length = std::sqrt(lengthSquared);

            if (length <= T(0))
            {
                return result;
            }

            const T inverse = T(1) / length;

            for (std::uint32_t index = 0; index < O; index++)
            {
                result[index] = (*this)[index] * inverse;
            }

            return result;
        }

        inline T dot(const Vec<O, T, Q>& inValue) const
        {
            T result = T(0);

            for (std::uint32_t index = 0; index < O; index++)
            {
                result += (*this)[index] * inValue[index];
            }

            return result;
        }

        inline Vec<O, T, Q> abs() const
        {
            Vec<O, T, Q> result(T(0));

            for (std::uint32_t index = 0; index < O; index++)
            {
                const T component = (*this)[index];

                result[index] = component < T(0) ? -component : component;
            }

            return result;
        }

        inline Vec<O, T, Q> min(const Vec<O, T, Q>& inValue) const
        {
            Vec<O, T, Q> result(T(0));

            for (std::uint32_t index = 0; index < O; index++)
            {
                const T left  = (*this)[index];
                const T right = inValue[index];

                result[index] = left < right ? left : right;
            }

            return result;
        }

        inline Vec<O, T, Q> max(const Vec<O, T, Q>& inValue) const
        {
            Vec<O, T, Q> result(T(0));

            for (std::uint32_t index = 0; index < O; index++)
            {
                const T left  = (*this)[index];
                const T right = inValue[index];

                result[index] = left > right ? left : right;
            }

            return result;
        }
    };
}