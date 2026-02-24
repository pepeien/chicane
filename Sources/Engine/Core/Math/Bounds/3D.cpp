#include "Chicane/Core/Math/Bounds/3D.hpp"

namespace Chicane
{
    Bounds3D::Bounds3D(const Vertex::List& inVertices)
        : Bounds3D()
    {
        add(inVertices);
    }

    Bounds3D::Bounds3D()
        : m_min(Vec3(FLT_MAX), Vec3::Zero),
          m_max(Vec3(-FLT_MAX), Vec3::Zero),
          m_top(Vec3::Zero),
          m_center(Vec3::Zero),
          m_bottom(Vec3::Zero),
          m_size(Vec3::One),
          m_corners({}),
          m_indices({})
    {
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

    bool Bounds3D::intersects(const Bounds3D& inOther) const
    {
        const Vec3& aMin = getMin().transformed;
        const Vec3& aMax = getMax().transformed;
        const Vec3& bMin = inOther.getMin().transformed;
        const Vec3& bMax = inOther.getMax().transformed;

        return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
               (aMin.z <= bMax.z && aMax.z >= bMin.z);
    }

    bool Bounds3D::intersects(const Vec3& inPoint) const
    {
        const Vec3& min = getMin().transformed;
        const Vec3& max = getMax().transformed;

        const bool bIsWithinX = inPoint.x >= min.x && inPoint.x <= max.x;
        const bool bIsWithinY = inPoint.y >= min.y && inPoint.y <= max.y;
        const bool bIsWithinZ = inPoint.z >= min.z && inPoint.z <= max.z;

        return bIsWithinX && bIsWithinY && bIsWithinZ;
    }

    const Vec3& Bounds3D::getTop() const
    {
        return m_top;
    }

    const Vec3& Bounds3D::getCenter() const
    {
        return m_center;
    }

    const Vec3& Bounds3D::getBottom() const
    {
        return m_bottom;
    }

    const Vec3& Bounds3D::getSize() const
    {
        return m_size;
    }

    const Bounds3DCorner& Bounds3D::getMin() const
    {
        return m_min;
    }

    const Bounds3DCorner& Bounds3D::getMax() const
    {
        return m_max;
    }

    const Vertex::List& Bounds3D::getCorners() const
    {
        return m_corners;
    }

    const Vertex::Indices& Bounds3D::getIndices() const
    {
        return m_indices;
    }

    void Bounds3D::add(const Vertex::List& inVertices)
    {
        for (const Vertex& vertex : inVertices)
        {
            m_min.base.x = std::min(m_min.base.x, vertex.position.x);
            m_min.base.y = std::min(m_min.base.y, vertex.position.y);
            m_min.base.z = std::min(m_min.base.z, vertex.position.z);

            m_max.base.x = std::max(m_max.base.x, vertex.position.x);
            m_max.base.y = std::max(m_max.base.y, vertex.position.y);
            m_max.base.z = std::max(m_max.base.z, vertex.position.z);
        }

        refreshCorners(m_baseCorners, m_min.base, m_max.base);
    }

    void Bounds3D::add(const Bounds3D& inBounds)
    {
        const Vec3& min = inBounds.getMin().base;
        m_min.base.x    = std::min(m_min.base.x, min.x);
        m_min.base.y    = std::min(m_min.base.y, min.y);
        m_min.base.z    = std::min(m_min.base.z, min.z);

        const Vec3& max = inBounds.getMax().base;
        m_max.base.x    = std::max(m_max.base.x, max.x);
        m_max.base.y    = std::max(m_max.base.y, max.y);
        m_max.base.z    = std::max(m_max.base.z, max.z);

        refreshCorners(m_baseCorners, m_min.base, m_max.base);
    }

    void Bounds3D::transform(const Mat4& inModel)
    {
        if (m_baseCorners.empty())
        {
            return;
        }

        Vec3 min(FLT_MAX);
        Vec3 max(-FLT_MAX);

        for (const Vertex& corner : m_baseCorners)
        {
            Vec3 position = inModel * Vec4(corner.position, 1.0f);

            min = min.min(position);
            max = max.max(position);
        }

        m_min.transformed = min;
        m_max.transformed = max;

        m_center.x = (m_min.transformed.x + m_max.transformed.x) * 0.5f;
        m_center.y = (m_min.transformed.y + m_max.transformed.y) * 0.5f;
        m_center.z = (m_min.transformed.z + m_max.transformed.z) * 0.5f;

        m_top.x = m_center.x;
        m_top.y = m_center.y;
        m_top.z = m_max.transformed.z;

        m_bottom.x = m_center.x;
        m_bottom.y = m_center.y;
        m_bottom.z = m_min.transformed.z;

        m_size = m_max.transformed - m_min.transformed;

        refreshCorners(m_corners, m_min.transformed, m_max.transformed);
    }

    void Bounds3D::refreshCorners(Vertex::List& outValue, const Vec3& inMin, const Vec3& inMax) const
    {
        //            max
        //    +------+       7+------+6
        //   /| top /|       /|     /|
        //  +------+ |     3+------+2|
        //  | |cent| |      | |    | |
        //  | +----|-+      |4+----|-+5
        //  |/ bot |/       |/     |/
        //  +------+       0+------+1
        // min

        outValue = {
            {.position = {inMin.x, inMin.y, inMin.z}, .color = Vec4(255.0f)}, // 0
            {.position = {inMax.x, inMin.y, inMin.z}, .color = Vec4(255.0f)}, // 1
            {.position = {inMax.x, inMax.y, inMin.z}, .color = Vec4(255.0f)}, // 2
            {.position = {inMin.x, inMax.y, inMin.z}, .color = Vec4(255.0f)}, // 3
            {.position = {inMin.x, inMin.y, inMax.z}, .color = Vec4(255.0f)}, // 4
            {.position = {inMax.x, inMin.y, inMax.z}, .color = Vec4(255.0f)}, // 5
            {.position = {inMax.x, inMax.y, inMax.z}, .color = Vec4(255.0f)}, // 6
            {.position = {inMin.x, inMax.y, inMax.z}, .color = Vec4(255.0f)}, // 7
        };
    }
}