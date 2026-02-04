#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE SpatialTransform : protected Transform
    {
    public:
        // Transformation
        const Mat4& getMatrix() const;

        const Vec3& getTranslation() const;
        const Rotator& getRotation() const;
        const Vec3& getScale() const;

        const Vec3& getRelativeTranslation() const;
        void addRelativeTranslation(const Vec3& inTranslation);
        void setRelativeTranslation(const Vec3& inTranslation);

        const Rotator& getRelativeRotation() const;
        void addRelativeRotation(const Rotator& inRotation);
        void setRelativeRotation(const Rotator& inRotation);

        const Vec3& getRelativeScale() const;
        void addRelativeScale(const Vec3& inScale);
        void setRelativeScale(const Vec3& inScale);

        const Vec3& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(const Vec3& inTranslation);
        void setAbsoluteTranslation(const Vec3& inTranslation);

        const Rotator& getAbsoluteRotation() const;
        void addAbsoluteRotation(const Rotator& inRotation);
        void setAbsoluteRotation(const Rotator& inRotation);

        const Vec3& getAbsoluteScale() const;
        void addAbsoluteScale(const Vec3& inScale);
        void setAbsoluteScale(const Vec3& inScale);

    private:
        void refresh();

    private:
        Transform m_relative = {};
        Transform m_absolute = {};
    };
}