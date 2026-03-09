#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
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

        void set(float inValue);
        void set(float inPitch, float inRoll, float inYaw);
        void set(const Vec3& inValue);
        void set(const Rotator& inValue);
        void set(const QuatFloat& inValue);

        void addLocal(float inValue);
        void addLocal(float inPitch, float inRoll, float inYaw);
        void addLocal(const Vec3& inValue);
        void addLocal(const Rotator& inValue);
        void addLocal(const QuatFloat& inDelta);

        void addWorld(float inValue);
        void addWorld(float inPitch, float inRoll, float inYaw);
        void addWorld(const Vec3& inValue);
        void addWorld(const Rotator& inValue);
        void addWorld(const QuatFloat& inDelta);

        void lookAt(const Vec3& inOrigin, const Vec3& inTarget);

        // Euler
        const Vec3& getAngles() const;
        void setAngles(const Vec3& inAngles);

        // Axis
        const Vec3& getRight() const;
        const Vec3& getForward() const;
        const Vec3& getUp() const;

    private:
        void refresh();

    public:
        CH_FIELD()
        Vec3 angles;

        CH_FIELD()
        Vec3 right;

        CH_FIELD()
        Vec3 forward;

        CH_FIELD()
        Vec3 up;

    private:
        QuatFloat m_orientation;
    };
}