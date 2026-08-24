#include "Chicane/Core/Math/Transform/2D/Spatial.reflected.hpp"

namespace Chicane
{
    namespace
    {
        Vec2 rotate(const Vec2& inValue, float inDegrees)
        {
            const float radians = glm::radians(inDegrees);
            const float cosine  = glm::cos(radians);
            const float sine    = glm::sin(radians);

            return Vec2((inValue.x * cosine) - (inValue.y * sine), (inValue.x * sine) + (inValue.y * cosine));
        }
    }

    SpatialTransform2D::SpatialTransform2D()
        : m_relative({}),
          m_absolute({})
    {}

    const Vec2& SpatialTransform2D::getRelativeTranslation() const
    {
        return m_relative.getTranslation();
    }

    float SpatialTransform2D::getRelativeRotation() const
    {
        return m_relative.getRotation();
    }

    const Vec2& SpatialTransform2D::getRelativeScale() const
    {
        return m_relative.getScale();
    }

    const Vec2& SpatialTransform2D::getAbsoluteTranslation() const
    {
        return m_absolute.getTranslation();
    }

    float SpatialTransform2D::getAbsoluteRotation() const
    {
        return m_absolute.getRotation();
    }

    const Vec2& SpatialTransform2D::getAbsoluteScale() const
    {
        return m_absolute.getScale();
    }

    void SpatialTransform2D::lookAt(const Vec2& inTarget)
    {
        m_absolute.lookAt(inTarget);

        refresh();
    }

    void SpatialTransform2D::refresh()
    {
        const float worldRotation = m_absolute.getRotation() + m_relative.getRotation();
        const Vec2  localOffset   = m_relative.getTranslation() * m_absolute.getScale();

        translation = m_absolute.getTranslation() + rotate(localOffset, m_absolute.getRotation());
        rotation    = worldRotation;
        scale       = m_absolute.getScale() * m_relative.getScale();

        Transform2D::refresh();

        onTransformChanged();

        emmitChanges();
    }
}
