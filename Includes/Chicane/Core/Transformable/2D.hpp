#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Transform/2D/Spatial.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_CORE Transformable2D : public SpatialTransform2D
    {
    public:
        Transformable2D();

    protected:
        inline virtual void onTransform() { return; }

        void onTransformChanged() override;

    public:
        const Bounds2D& getBounds() const;
        void addBounds(const Bounds2D& inBounds);

    protected:
        Bounds2D m_bounds;
    };
}
