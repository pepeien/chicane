#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Changeable.hpp"
#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"

namespace Chicane
{
    class CHICANE_CORE Transformable : public Changeable
    {
    public:
        Transformable();

    protected:
        inline virtual void onTransform() { return; }

    public:
        // Transform
        const SpatialTransform& getTransform() const;

        const Vec3& getTranslation() const;
        const Rotator& getRotation() const;
        const Vec3& getScale() const;

        const Vec3& getForward() const;
        const Vec3& getRight() const;
        const Vec3& getUp() const;

        // Relative
        const Vec3& getRelativeTranslation() const;
        void addRelativeTranslation(float inValue);
        void addRelativeTranslation(float inX, float inY, float inZ);
        void addRelativeTranslation(const Vec3& inTranslation);
        void setRelativeTranslation(float inValue);
        void setRelativeTranslation(float inX, float inY, float inZ);
        void setRelativeTranslation(const Vec3& inTranslation);

        const Rotator& getRelativeRotation() const;
        void addRelativeRotation(float inValue);
        void addRelativeRotation(float inPitch, float inRoll, float inYaw);
        void addRelativeRotation(const Vec3& inAngles);
        void addRelativeRotation(const Rotator& inRotation);
        void setRelativeRotation(float inValue);
        void setRelativeRotation(float inPitch, float inRoll, float inYaw);
        void setRelativeRotation(const Vec3& inRotation);
        void setRelativeRotation(const Rotator& inRotation);

        const Vec3& getRelativeScale() const;
        void addRelativeScale(float inValue);
        void addRelativeScale(float inX, float inY, float inZ);
        void addRelativeScale(const Vec3& inScale);
        void setRelativeScale(float inValue);
        void setRelativeScale(float inX, float inY, float inZ);
        void setRelativeScale(const Vec3& inScale);

        // Absolute
        const Vec3& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(float inValue);
        void addAbsoluteTranslation(float inX, float inY, float inZ);
        void addAbsoluteTranslation(const Vec3& inTranslation);
        void setAbsoluteTranslation(float inValue);
        void setAbsoluteTranslation(float inX, float inY, float inZ);
        void setAbsoluteTranslation(const Vec3& inTranslation);

        const Rotator& getAbsoluteRotation() const;
        void addAbsoluteRotation(float inValue);
        void addAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void addAbsoluteRotation(const Vec3& inAngles);
        void addAbsoluteRotation(const Rotator& inRotation);
        void setAbsoluteRotation(float inValue);
        void setAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void setAbsoluteRotation(const Vec3& inAngles);
        void setAbsoluteRotation(const Rotator& inRotation);

        const Vec3& getAbsoluteScale() const;
        void addAbsoluteScale(float inValue);
        void addAbsoluteScale(float inX, float inY, float inZ);
        void addAbsoluteScale(const Vec3& inScale);
        void setAbsoluteScale(float inValue);
        void setAbsoluteScale(float inX, float inY, float inZ);
        void setAbsoluteScale(const Vec3& inScale);

        // Bounds
        const Bounds3D& getBounds() const;
        void addBounds(const Bounds3D& inBounds);
        void setBounds(const Bounds3D& inBounds);

        const Vec3& getTop() const;
        const Vec3& getCenter() const;
        const Vec3& getBottom() const;

    private:
        void refresh();

    private:
        // Transform
        SpatialTransform m_transform;

        // Bounds
        Bounds3D         m_bounds;
    };
}