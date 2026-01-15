#pragma once

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME RendererView
    {
    public:
        // Settings
        Vec4 clip        = Vec4(0.1f, 1000.0f, 0.0f, 0.0f);

        // Matrices
        Mat4 projection  = Mat4::One;
        Mat4 view        = Mat4::One;

        // Orientantion
        Vec4 up          = Vec4::Up;
        Vec4 right       = Vec4::Right;
        Vec4 forward     = Vec4::Forward;

        // Transform
        Vec4 translation = Vec4::Zero;
    };
}