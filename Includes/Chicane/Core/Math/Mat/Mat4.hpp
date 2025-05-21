#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Mat/Base.hpp"

namespace Chicane
{
    struct CHICANE_CORE Mat4 : public Mat<4, float>
    {
    public:
        static const Mat4 Zero;
        static const Mat4 One;

    public:
        template<typename... A>
        Mat4(A ...args)
            : Mat<4, float>(args...)
        {}
    };
}