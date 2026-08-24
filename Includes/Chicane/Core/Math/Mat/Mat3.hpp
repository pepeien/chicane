#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat.hpp"

namespace Chicane
{
    struct CHICANE_CORE Mat3 : public Mat<3, float>
    {
    public:
        static constexpr inline const Mat<3, float> Zero = Mat<3, float>(0.0f);
        static constexpr inline const Mat<3, float> One  = Mat<3, float>(1.0f);

    public:
        template <typename... A>
        constexpr Mat3(A... args)
            : Mat<3, float>(args...)
        {}
    };
}
