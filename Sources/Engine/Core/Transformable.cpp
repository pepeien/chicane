#include "Chicane/Core/Transformable.reflected.hpp"

namespace Chicane
{
    Transformable::Transformable()
        : SpatialTransform(),
          m_bounds()
    {}

    void Transformable::onRefresh()
    {
        m_bounds.transform(getMatrix());

        onTransform();
    }

    const Bounds3D& Transformable::getBounds() const
    {
        return m_bounds;
    }

    void Transformable::addBounds(const Bounds3D& inBounds)
    {
        m_bounds.add(inBounds);
        m_bounds.transform(getMatrix());
    }

    const Vec3& Transformable::getTop() const
    {
        return m_bounds.getTop();
    }

    const Vec3& Transformable::getCenter() const
    {
        return m_bounds.getCenter();
    }

    const Vec3& Transformable::getBottom() const
    {
        return m_bounds.getBottom();
    }
}