#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Rotator
    {
    public:
        const QuatFloat& getOrientation() const;
        void setOrientation(const QuatFloat& inValue);

        const Vec3& getVector() const;
        void add(const Vec3& inRotation);
        void set(const Vec3& inRotation);

        const Vec3& getRight() const;
        const Vec3& getForward() const;
        const Vec3& getUp() const;

    protected:
        void refresh();

    protected:
        QuatFloat m_orientation = QuatFloat(1, 0, 0, 0);

        Vec3      m_euler       = Vec3::Zero;

        Vec3      m_forward     = Vec3::Forward;
        Vec3      m_up          = Vec3::Up;
        Vec3      m_right       = Vec3::Right;
    };
}