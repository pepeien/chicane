#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_CORE Transformable : public SpatialTransform
    {
    public:
        Transformable();

    protected:
        inline virtual void onTransform() { return; }

    protected:
        void onRefresh() override;

    public:
        // Bounds
        const Bounds3D& getBounds() const;
        void addBounds(const Bounds3D& inBounds);

        const Vec3& getTop() const;
        const Vec3& getCenter() const;
        const Vec3& getBottom() const;

    private:
        // Bounds
        Bounds3D m_bounds;
    };
}