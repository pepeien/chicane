#include "Chicane/Core/Math/Bounds/3D.hpp"

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
          m_size(Vec3::One),
          m_positions({}),
          m_indices({})
    {}

    bool Bounds3D::intersects(const Bounds3D& inOther) const
    {
        const Vec3& aMin = getMin();
        const Vec3& aMax = getMax();
        const Vec3& bMin = inOther.getMin();
        const Vec3& bMax = inOther.getMax();

        return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
               (aMin.z <= bMax.z && aMax.z >= bMin.z);
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

        m_size.x = m_max.x - m_min.x;
        m_size.y = m_max.y - m_min.y;
        m_size.z = m_max.z - m_min.z;

        if (m_positions.empty())
        {
            m_positions.resize(8);
        }

        //   7+------+6
        //   /|     /|
        // 3+------+2|
        //  | |    | |
        //  |4+----|-+5
        //  |/     |/
        // 0+------+1

        m_positions = {
            {m_min.x, m_min.y, m_min.z}, // 0 left  front bottom
            {m_max.x, m_min.y, m_min.z}, // 1 right front bottom
            {m_max.x, m_max.y, m_min.z}, // 2 right back  bottom
            {m_min.x, m_max.y, m_min.z}, // 3 left  back  bottom
            {m_min.x, m_min.y, m_max.z}, // 4 left  front top
            {m_max.x, m_min.y, m_max.z}, // 5 right front top
            {m_max.x, m_max.y, m_max.z}, // 6 right back  top
            {m_min.x, m_max.y, m_max.z}, // 7 left  back  top
        };

        m_indices = {
            // Bottom face (z = m_min.z): 0, 1, 2, 3
            0,
            1,
            2,
            0,
            2,
            3,

            // Top face    (z = m_max.z): 4, 5, 6, 7
            4,
            5,
            6,
            4,
            6,
            7,

            // Front face  (y = m_min.y): 0, 1, 4, 5
            0,
            1,
            5,
            0,
            5,
            4,

            // Back face   (y = m_max.y): 2, 3, 6, 7
            2,
            3,
            7,
            2,
            7,
            6,

            // Left face   (x = m_min.x): 0, 3, 4, 7
            0,
            3,
            7,
            0,
            7,
            4,

            // Right face  (x = m_max.x): 1, 2, 5, 6
            1,
            2,
            6,
            1,
            6,
            5,
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

    const Vec3& Bounds3D::getSize() const
    {
        return m_size;
    }

    const Vertex::Positions& Bounds3D::getPositions() const
    {
        return m_positions;
    }

    const Vertex::Indices& Bounds3D::getIndices() const
    {
        return m_indices;
    }

    Vertex::List Bounds3D::getVertices() const
    {
        Vertex::List result;

        Vertex vertex;
        vertex.color  = Vec4(255.0f);
        vertex.normal = Vec3::Zero;
        vertex.uv     = Vec3::Zero;

        for (const Vertex::Position& position : m_positions)
        {
            vertex.position.x = position.x;
            vertex.position.y = position.y;
            vertex.position.z = position.z;

            result.push_back(vertex);
        }

        return result;
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