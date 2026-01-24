#include "Chicane/Core/Math/Bounds/Points.hpp"

namespace Chicane
{
    BoundsPoints::BoundsPoints(const Vertex::List& inVertices)
        : BoundsPoints()
    {
        m_min = Vec3(FLT_MAX);
        m_max = Vec3(-FLT_MAX);

        for (const Vertex& vertex : inVertices)
        {
            const Vec3& position = vertex.position;

            m_min.x = std::min(m_min.x, position.x);
            m_min.y = std::min(m_min.y, position.y);
            m_min.z = std::min(m_min.z, position.z);

            m_max.x = std::max(m_max.x, position.x);
            m_max.y = std::max(m_max.y, position.y);
            m_max.z = std::max(m_max.z, position.z);
        }
    }

    BoundsPoints::BoundsPoints()
        : m_min(Vec3::Zero),
          m_max(Vec3::Zero),
          m_top(Vec3::Zero),
          m_center(Vec3::Zero),
          m_bottom(Vec3::Zero)
    {}

    void BoundsPoints::add(const Vec3& inMin, const Vec3& inMax)
    {
        m_min.x = std::min(m_min.x, inMin.x);
        m_min.y = std::min(m_min.y, inMin.y);
        m_min.z = std::min(m_min.z, inMin.z);

        m_max.x = std::max(m_max.x, inMax.x);
        m_max.y = std::max(m_max.y, inMax.y);
        m_max.z = std::max(m_max.z, inMax.z);

        m_center.x = (m_min.x + m_max.x) * 0.5f;
        m_center.y = (m_min.y + m_max.y) * 0.5f;
        m_center.z = (m_min.z + m_max.z) * 0.5f;

        m_top.x = m_center.x;
        m_top.y = m_center.y;
        m_top.z = m_max.z;

        m_bottom.x = m_center.x;
        m_bottom.y = m_center.y;
        m_bottom.z = m_min.z;
    }

    const Vec3& BoundsPoints::getMin() const
    {
        return m_min;
    }

    void BoundsPoints::setMin(const Vec3& inValue)
    {
        m_min = inValue;
    }

    const Vec3& BoundsPoints::getMax() const
    {
        return m_max;
    }

    void BoundsPoints::setMax(const Vec3& inValue)
    {
        m_max = inValue;
    }

    const Vec3& BoundsPoints::getTop() const
    {
        return m_top;
    }

    void BoundsPoints::setTop(const Vec3& inValue)
    {
        m_top = inValue;
    }

    const Vec3& BoundsPoints::getCenter() const
    {
        return m_center;
    }

    void BoundsPoints::setCenter(const Vec3& inValue)
    {
        m_center = inValue;
    }

    const Vec3& BoundsPoints::getBottom() const
    {
        return m_bottom;
    }

    void BoundsPoints::setBottom(const Vec3& inValue)
    {
        m_bottom = inValue;
    }
}