#pragma once

#include "Chicane/Core/Event/Changeable.hpp"
#include "Chicane/Core/Math/Bounds.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Transformable : public Changeable
    {
    public:
        Transformable();

    public:
        // Transform
        const SpatialTransform& getTransform() const;

        const Vec3& getTranslation() const;
        const Vec3& getRotation() const;
        const Vec3& getScale() const;

        const Vec3& getForward() const;
        const Vec3& getRight() const;
        const Vec3& getUp() const;

        // Relative
        const Transform& getRelative() const;
        void setRelative(const Transform& inTransform);

        const Vec3& getRelativeTranslation() const;
        void addRelativeTranslation(float inValue);
        void addRelativeTranslation(float inX, float inY, float inZ);
        void addRelativeTranslation(const Vec3& inTranslation);
        void setRelativeTranslation(float inValue);
        void setRelativeTranslation(float inX, float inY, float inZ);
        void setRelativeTranslation(const Vec3& inTranslation);

        const Vec3& getRelativeRotation() const;
        void addRelativeRotation(float inValue);
        void addRelativeRotation(float inPitch, float inRoll, float inYaw);
        void addRelativeRotation(const Vec3& inRotation);
        void setRelativeRotation(float inValue);
        void setRelativeRotation(float inPitch, float inRoll, float inYaw);
        void setRelativeRotation(const Vec3& inRotation);

        const Vec3& getRelativeScale() const;
        void addRelativeScale(float inValue);
        void addRelativeScale(float inX, float inY, float inZ);
        void addRelativeScale(const Vec3& inScale);
        void setRelativeScale(float inValue);
        void setRelativeScale(float inX, float inY, float inZ);
        void setRelativeScale(const Vec3& inScale);

        // Absolute
        const Transform& getAbsolute() const;
        void setAbsolute(const Transform& inTransform);

        const Vec3& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(float inValue);
        void addAbsoluteTranslation(float inX, float inY, float inZ);
        void addAbsoluteTranslation(const Vec3& inTranslation);
        void setAbsoluteTranslation(float inValue);
        void setAbsoluteTranslation(float inX, float inY, float inZ);
        void setAbsoluteTranslation(const Vec3& inTranslation);

        const Vec3& getAbsoluteRotation() const;
        void addAbsoluteRotation(float inValue);
        void addAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void addAbsoluteRotation(const Vec3& inRotation);
        void setAbsoluteRotation(float inValue);
        void setAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void setAbsoluteRotation(const Vec3& inRotation);

        const Vec3& getAbsoluteScale() const;
        void addAbsoluteScale(float inValue);
        void addAbsoluteScale(float inX, float inY, float inZ);
        void addAbsoluteScale(const Vec3& inScale);
        void setAbsoluteScale(float inValue);
        void setAbsoluteScale(float inX, float inY, float inZ);
        void setAbsoluteScale(const Vec3& inScale);

        // Bounds
        const Bounds& getBounds() const;
        void addBounds(const Bounds& inBounds);
        void setBounds(const Bounds& inBounds);

        const Vec3& getTop() const;
        const Vec3& getCenter() const;
        const Vec3& getBottom() const;

    private:
        void refresh();

    private:
        // Transform
        SpatialTransform m_transform;

        // Bounds
        Bounds           m_bounds;
    };
}