#pragma once

#include <vector>

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    struct CHICANE_CORE View
    {
    public:
        using List = std::vector<View>;

    public:
        void flipY();
        void depthZeroToOne();

    public:
        // Settings
        Vec4 clip = Vec4(0.1f, 100.0f, 0.0f, 0.0f);

        // Matrices
        Mat4 view       = Mat4::One;
        Mat4 projection = Mat4::One;

        // Orientantion
        Vec4 up      = Vec4::sUp();
        Vec4 right   = Vec4::sRight();
        Vec4 forward = Vec4::sForward();

        // Transform
        Vec4 translation = Vec4::sZero();
    };
}
