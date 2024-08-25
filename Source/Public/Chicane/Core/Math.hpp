#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Vec : glm::vec<O, T, Q>
    {
        template<typename... A>
        Vec(A ...args)
            : glm::vec<O, T, Q>(args...)
        {}
    };

    
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Mat : glm::mat<O, O, T, Q>
    {
        template<typename... A>
        Mat(A ...args)
            : glm::mat<O, O, T, Q>(args...)
        {}
    };

    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct Quat : glm::qua<T, Q>
    {
        template<typename... A>
        Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };

    struct Transform
    {
        Vec<3, float> translation = Vec<3, float>(0.0f);
        Vec<3, float> rotation    = Vec<3, float>(0.0f);
        Vec<3, float> scale       = Vec<3, float>(1.0f);
    };
}