#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE TransformCombined : protected Transform
    {
    public:
        // Transformation
        const Mat4& getMatrix() const;

        const Vec3& getTranslation() const;
        const Vec3& getRotation() const;
        const Vec3& getScale() const;

        const Transform& getRelativeTransform() const;
        void setRelativeTransform(const Transform& inTransform);

        const Vec3& getRelativeTranslation() const;
        void addRelativeTranslation(const Vec3& inTranslation);
        void setRelativeTranslation(const Vec3& inTranslation);

        const Vec3& getRelativeRotation() const;
        void addRelativeRotation(const Vec3& inRotation);
        void setRelativeRotation(const Vec3& inRotation);

        const Vec3& getRelativeScale() const;
        void addRelativeScale(const Vec3& inScale);
        void setRelativeScale(const Vec3& inScale);

        const Transform& getAbsoluteTransform() const;
        void setAbsoluteTransform(const Transform& inTransform);

        const Vec3& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(const Vec3& inTranslation);
        void setAbsoluteTranslation(const Vec3& inTranslation);

        const Vec3& getAbsoluteRotation() const;
        void addAbsoluteRotation(const Vec3& inRotation);
        void setAbsoluteRotation(const Vec3& inRotation);

        const Vec3& getAbsoluteScale() const;
        void addAbsoluteScale(const Vec3& inScale);
        void setAbsoluteScale(const Vec3& inScale);

        // Orientation
        const Vec3& getRight() const;
        const Vec3& getForward() const;
        const Vec3& getUp() const;

    private:
        void refresh();

    private:
        Transform m_relative = {};
        Transform m_absolute = {};
    };
}