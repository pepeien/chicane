#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat.hpp"

namespace Chicane
{
    struct CHICANE_CORE Mat4 : public Mat<4, float>
    {
    public:
        static inline constexpr const Mat<4, float> Zero = Mat<4, float>(0.0f);
        static inline constexpr const Mat<4, float> One  = Mat<4, float>(1.0f);

    public:
        template<typename... A>
        constexpr Mat4(A ...args)
            : Mat<4, float>(args...)
        {}
    };
}