#include "Chicane/Core/Transformable/2D.reflected.hpp"

namespace Chicane
{
    Transformable2D::Transformable2D()
        : SpatialTransform2D(),
          m_bounds({})
    {}

    void Transformable2D::onTransformChanged()
    {
        m_bounds.transform(getMatrix());

        onTransform();
    }

    const Bounds2D& Transformable2D::getBounds() const
    {
        return m_bounds;
    }

    void Transformable2D::addBounds(const Bounds2D& inBounds)
    {
        m_bounds.add(inBounds);
        m_bounds.transform(getMatrix());
    }
}
