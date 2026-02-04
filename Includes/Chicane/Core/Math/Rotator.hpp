#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Rotator
    {
    public:
        Rotator(float inAngle);
        Rotator(float inPitch, float inRoll, float inYaw);
        Rotator(const Vec3& inAngles);
        Rotator(const QuatFloat& inOrientation);
        Rotator();

    public:
        // Orientation
        const QuatFloat& get() const;
        void set(const QuatFloat& inValue);
        void addLocal(const QuatFloat& inDelta);
        void addWorld(const QuatFloat& inDelta);

        // Euler
        const Vec3& getAngles() const;
        void setAngles(const Vec3& inAngles);

        // Axis
        const Vec3& getRight() const;
        const Vec3& getForward() const;
        const Vec3& getUp() const;

    private:
        void refresh();

    private:
        QuatFloat m_orientation;

        Vec3      m_angles;

        Vec3      m_right;
        Vec3      m_forward;
        Vec3      m_up;
    };
}