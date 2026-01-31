#include "Chicane/Core/Math/Bounds/3D.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;

namespace Chicane
{
    Bounds3D::Bounds3D(const Vertex::List& inVertices, const Vertex::Indices& inIndices)
        : Bounds3D()
    {
        for (std::uint32_t index : inIndices)
        {
            const Vertex& vertex = inVertices.at(index);

            const Vec3& position = vertex.position;

            m_baseMin.x = std::min(m_baseMin.x, position.x);
            m_baseMin.y = std::min(m_baseMin.y, position.y);
            m_baseMin.z = std::min(m_baseMin.z, position.z);

            m_baseMax.x = std::max(m_baseMax.x, position.x);
            m_baseMax.y = std::max(m_baseMax.y, position.y);
            m_baseMax.z = std::max(m_baseMax.z, position.z);
        }

        refresh();
    }

    Bounds3D::Bounds3D()
        : m_min(Vec3::Zero),
          m_baseMin(Vec3(FLT_MAX)),
          m_max(Vec3::Zero),
          m_baseMax(Vec3(-FLT_MAX)),
          m_top(Vec3::Zero),
          m_baseTop(Vec3::Zero),
          m_center(Vec3::Zero),
          m_baseCenter(Vec3::Zero),
          m_bottom(Vec3::Zero),
          m_baseBottom(Vec3::Zero),
          m_vertices({}),
          m_indices({})
    {}

    bool Bounds3D::intersects(const Bounds3D& inBounds) const
    {
        for (const Vec3& corner : inBounds.getVertices())
        {
            if (intersects(corner))
            {
                return true;
            }
        }

        return false;
    }

    bool Bounds3D::intersects(const Vec3& inPoint) const
    {
        const Vec3& min = getMin();
        const Vec3& max = getMax();

        const bool bIsWithinX = inPoint.x >= min.x && inPoint.x <= max.x;
        const bool bIsWithinY = inPoint.y >= min.y && inPoint.y <= max.y;
        const bool bIsWithinZ = inPoint.z >= min.z && inPoint.z <= max.z;

        return bIsWithinX && bIsWithinY && bIsWithinZ;
    }

    Vec3 Bounds3D::getOverlap(const Bounds3D& inOther) const
    {
        const Vec3& min = getMin();
        const Vec3& max = getMax();

        const Vec3& otherMin = inOther.getMin();
        const Vec3& otherMax = inOther.getMax();

        float overlapX = std::min(max.x, otherMax.x) - std::max(min.x, otherMin.x);
        float overlapY = std::min(max.y, otherMax.y) - std::max(min.y, otherMin.y);
        float overlapZ = std::min(max.z, otherMax.z) - std::max(min.z, otherMin.z);

        if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0)
        {
            return Vec3::Zero;
        }

        Vec3 result = Vec3::Zero;

        if (overlapX < overlapY && overlapX < overlapZ)
        {
            result.x = overlapX;
        }
        else if (overlapY < overlapX && overlapY < overlapZ)
        {
            result.y = overlapY;
        }
        else
        {
            result.z = overlapZ;
        }

        return result;
    }

    void Bounds3D::set(const Bounds3D& inBounds)
    {
        m_baseMin = inBounds.getBaseMin();
        m_baseMax = inBounds.getBaseMax();

        refresh();
    }

    void Bounds3D::add(const Bounds3D& inBounds)
    {
        const Vec3& min = inBounds.getBaseMin();
        m_baseMin.x     = std::min(m_baseMin.x, min.x);
        m_baseMin.y     = std::min(m_baseMin.y, min.y);
        m_baseMin.z     = std::min(m_baseMin.z, min.z);

        const Vec3& max = inBounds.getBaseMax();
        m_baseMax.x     = std::max(m_baseMax.x, max.x);
        m_baseMax.y     = std::max(m_baseMax.y, max.y);
        m_baseMax.z     = std::max(m_baseMax.z, max.z);

        refresh();
    }

    void Bounds3D::update(const SpatialTransform& inTransform)
    {
        const Vec3& scale       = inTransform.getScale();
        const Vec3& translation = inTransform.getTranslation();

        //            max
        //    +------+
        //   /| top /|
        //  +------+ |
        //  | |cent| |
        //  | +----|-+
        //  |/ bot |/
        //  +------+
        // min

        m_min = (m_baseMin * scale) + translation;
        m_max = (m_baseMax * scale) + translation;

        m_center.x = (m_min.x + m_max.x) * 0.5f;
        m_center.y = (m_min.y + m_max.y) * 0.5f;
        m_center.z = (m_min.z + m_max.z) * 0.5f;

        m_top.x = m_center.x;
        m_top.y = m_center.y;
        m_top.z = m_max.z;

        m_bottom.x = m_center.x;
        m_bottom.y = m_center.y;
        m_bottom.z = m_min.z;

        if (m_vertices.empty())
        {
            m_vertices.resize(8);
        }

        //   7+------+6
        //   /|     /|
        // 3+------+2|
        //  | |    | |
        //  |4+----|-+5
        //  |/     |/
        // 0+------+1

        m_vertices = {
            {m_baseMin.x, m_baseMin.y, m_baseMin.z}, // 0
            {m_baseMax.x, m_baseMin.y, m_baseMin.z}, // 1
            {m_baseMin.x, m_baseMax.y, m_baseMin.z}, // 2
            {m_baseMax.x, m_baseMax.y, m_baseMin.z}, // 3
            {m_baseMin.x, m_baseMin.y, m_baseMax.z}, // 4
            {m_baseMax.x, m_baseMin.y, m_baseMax.z}, // 5
            {m_baseMin.x, m_baseMax.y, m_baseMax.z}, // 6
            {m_baseMax.x, m_baseMax.y, m_baseMax.z}  // 7
        };

        m_indices = {
            // Front face  (z = m_min.z): 0, 1, 2, 2, 3, 0
            0,
            1,
            2,
            2,
            3,
            0,

            // Back face   (z = m_max.z): 4, 5, 6, 6, 7, 4
            4,
            5,
            6,
            6,
            7,
            4,

            // Left face   (x = m_min.x): 0, 4, 6, 6, 2, 0
            0,
            4,
            6,
            6,
            2,
            0,

            // Right face  (x = m_max.x): 1, 5, 7, 7, 3, 1
            1,
            5,
            7,
            7,
            3,
            1,

            // Top face    (y = m_max.y): 2, 6, 7, 7, 3, 2
            2,
            6,
            7,
            7,
            3,
            2,

            // Bottom face (y = m_min.y): 0, 1, 5, 5, 4, 0
            0,
            1,
            5,
            5,
            4,
            0
        };
    }

    const Vec3& Bounds3D::getMin() const
    {
        return m_min;
    }

    const Vec3& Bounds3D::getBaseMin() const
    {
        return m_baseMin;
    }

    const Vec3& Bounds3D::getMax() const
    {
        return m_max;
    }

    const Vec3& Bounds3D::getBaseMax() const
    {
        return m_baseMax;
    }

    const Vec3& Bounds3D::getTop() const
    {
        return m_top;
    }

    const Vec3& Bounds3D::getBaseTop() const
    {
        return m_baseTop;
    }

    const Vec3& Bounds3D::getCenter() const
    {
        return m_center;
    }

    const Vec3& Bounds3D::getBaseCenter() const
    {
        return m_baseCenter;
    }

    const Vec3& Bounds3D::getBottom() const
    {
        return m_bottom;
    }

    const Vec3& Bounds3D::getBaseBottom() const
    {
        return m_baseBottom;
    }

    const std::vector<Vec3>& Bounds3D::getVertices() const
    {
        return m_vertices;
    }

    const Vertex::Indices& Bounds3D::getIndices() const
    {
        return m_indices;
    }

    void Bounds3D::refresh()
    {
        m_baseCenter.x = (m_baseMin.x + m_baseMax.x) * 0.5f;
        m_baseCenter.y = (m_baseMin.y + m_baseMax.y) * 0.5f;
        m_baseCenter.z = (m_baseMin.z + m_baseMax.z) * 0.5f;

        m_baseTop.x = m_baseCenter.x;
        m_baseTop.y = m_baseCenter.y;
        m_baseTop.z = m_baseMax.z;

        m_baseBottom.x = m_baseCenter.x;
        m_baseBottom.y = m_baseCenter.y;
        m_baseBottom.z = m_baseMin.z;

        m_min    = m_baseMin;
        m_max    = m_baseMax;
        m_top    = m_baseTop;
        m_center = m_baseCenter;
        m_bottom = m_baseBottom;
    }
}