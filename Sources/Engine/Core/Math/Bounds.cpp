#include "Chicane/Core/Math/Bounds.hpp"

#include "Chicane/Core.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;

namespace Chicane
{
    Bounds::Bounds(const std::vector<Vertex>& inVertices)
    {
        m_baseMin = Vec3(FLT_MAX);
        m_baseMax = Vec3(-FLT_MAX);

        for (const Vertex& vertex : inVertices)
        {
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

    bool Bounds::contains(const Bounds& inBounds) const
    {
        for (const Vec3& corner : inBounds.getCorners())
        {
            if (contains(corner))
            {
                return true;
            }
        }

        return false;
    }

    bool Bounds::contains(const Vec3& inPoint) const
    {
        const Vec3& min = getMin();
        const Vec3& max = getMax();

        const bool bIsWithinX = inPoint.x >= min.x && inPoint.x <= max.x;
        const bool bIsWithinY = inPoint.y >= min.y && inPoint.y <= max.y;
        const bool bIsWithinZ = inPoint.z >= min.z && inPoint.z <= max.z;

        return bIsWithinX && bIsWithinY && bIsWithinZ;
    }

    Vec3 Bounds::getOverlap(const Bounds& inOther) const
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

    void Bounds::set(const Bounds& inBounds)
    {
        m_baseMin = inBounds.getBaseMin();
        m_baseMax = inBounds.getBaseMax();

        refresh();
    }

    void Bounds::add(const Bounds& inBounds)
    {
        const Vec3& min = inBounds.getBaseMin();
        m_baseMin.x = std::min(m_baseMin.x, min.x);
        m_baseMin.y = std::min(m_baseMin.y, min.y);
        m_baseMin.z = std::min(m_baseMin.z, min.z);

        const Vec3& max = inBounds.getBaseMax();
        m_baseMax.x = std::max(m_baseMax.x, max.x);
        m_baseMax.y = std::max(m_baseMax.y, max.y);
        m_baseMax.z = std::max(m_baseMax.z, max.z);

        refresh();
    }

    void Bounds::update(const TransformCombined& inTransform)
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

        if (m_corners.empty())
        {
            m_corners.resize(8);
        }

        //   7+------+6
        //   /|     /|
        // 3+------+2|
        //  | |    | |
        //  |4+----|-+5
        //  |/     |/
        // 0+------+1

        m_corners[0].x = m_min.x;
        m_corners[0].y = m_min.y;
        m_corners[0].z = m_min.z;

        m_corners[1].x = m_max.x;
        m_corners[1].y = m_min.y;
        m_corners[1].z = m_min.z;

        m_corners[2].x = m_min.x;
        m_corners[2].y = m_max.y;
        m_corners[2].z = m_min.z;

        m_corners[3].x = m_max.x;
        m_corners[3].y = m_max.y;
        m_corners[3].z = m_min.z;

        m_corners[4].x = m_min.x;
        m_corners[4].y = m_min.y;
        m_corners[4].z = m_max.z;

        m_corners[5].x = m_max.x;
        m_corners[5].y = m_min.y;
        m_corners[5].z = m_max.z;

        m_corners[6].x = m_min.x;
        m_corners[6].y = m_max.y;
        m_corners[6].z = m_max.z;

        m_corners[7].x = m_max.x;
        m_corners[7].y = m_max.y;
        m_corners[7].z = m_max.z;
    }

    const Vec3& Bounds::getMin() const
    {
        return m_min;
    }

    const Vec3& Bounds::getBaseMin() const
    {
        return m_baseMin;
    }

    const Vec3& Bounds::getMax() const
    {
        return m_max;
    }

    const Vec3& Bounds::getBaseMax() const
    {
        return m_baseMax;
    }

    const Vec3& Bounds::getTop() const
    {
        return m_top;
    }

    const Vec3& Bounds::getBaseTop() const
    {
        return m_baseCenter;
    }

    const Vec3& Bounds::getBaseCenter() const
    {
        return m_baseCenter;
    }

    const Vec3& Bounds::getCenter() const
    {
        return m_center;
    }

    const Vec3& Bounds::getBottom() const
    {
        return m_bottom;
    }

    const Vec3& Bounds::getBaseBottom() const
    {
        return m_baseBottom;
    }

    const std::vector<Vec3>& Bounds::getCorners() const
    {
        return m_corners;
    }

    void Bounds::refresh()
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