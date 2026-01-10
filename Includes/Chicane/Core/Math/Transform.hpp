#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Transform
    {
    public:
        // Transformation
        const Mat4& getMatrix() const;
        void setMatrix(const Mat4& inMatrix);

        void setTransform(const Transform& inTransform);

        const Vec3& getTranslation() const;
        void addTranslation(const Vec3& inTranslation);
        void setTranslation(const Vec3& inTranslation);

        const Vec3& getRotation() const;
        void addRotation(const Vec3& inRotation);
        void setRotation(const Vec3& inRotation);

        const Vec3& getScale() const;
        void addScale(const Vec3& inScale);
        void setScale(const Vec3& inScale);

        // Orientation
        const Vec3& getRight() const;
        const Vec3& getForward() const;
        const Vec3& getUp() const;

    protected:
        void refreshOrientation(const QuatFloat& inOrientation);
        void refreshTransform(const QuatFloat& inOrientation);
        void refresh();

    protected:
        // Transformation
        Mat4 m_matrix      = Mat4::One;
        Vec3 m_translation = Vec3::Zero;
        Vec3 m_rotation    = Vec3::Zero;
        Vec3 m_scale       = Vec3::One;

        // Orientation
        Vec3 m_forward     = Vec3::Forward;
        Vec3 m_up          = Vec3::Up;
        Vec3 m_right       = Vec3::Right;
    };
}