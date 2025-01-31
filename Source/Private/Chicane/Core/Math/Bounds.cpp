#include "Chicane/Core/Math/Bounds.hpp"

#include "Chicane/Core.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;

namespace Chicane
{
    Bounds::Bounds(const std::vector<Box::Model::Vertex>& inVertices)
    {
        m_baseMin = Vec<3, float>(FLT_MAX);
        m_baseMax = Vec<3, float>(-FLT_MAX);

        for (const Box::Model::Vertex& vertex : inVertices)
        {
            const Vec<3, float>& position = vertex.position;

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
        const Vec<3, float> currentMin = getMin();
        const Vec<3, float> currentMax = getMax();

        const Vec<3, float> targetMin = inBounds.getMin();
        const Vec<3, float> targetMax = inBounds.getMax();

        const bool isWithinX = targetMin.x >= currentMin.x && targetMax.x <= currentMax.x;
        const bool isWithinY = targetMin.y >= currentMin.y && targetMax.y <= currentMax.y;
        const bool isWithinZ = targetMin.z >= currentMin.z && targetMax.z <= currentMax.z;

        return isWithinX && isWithinY && isWithinZ;
    }

    bool Bounds::contains(const Vec<3, float>& inPoint) const
    {
        const Vec<3, float> min = getMin();
        const Vec<3, float> max = getMax();

        const bool isWithinX = inPoint.x >= min.x && inPoint.x <= max.x;
        const bool isWithinY = inPoint.y >= min.y && inPoint.y <= max.y;
        const bool isWithinZ = inPoint.z >= min.z && inPoint.z <= max.z;

        return isWithinX && isWithinY && isWithinZ;
    }

    void Bounds::set(const Bounds& inBounds)
    {
        m_baseMin = inBounds.getBaseMin();
        m_baseMax = inBounds.getBaseMax();

        refresh();
    }

    void Bounds::add(const Bounds& inBounds)
    {
        const Vec<3, float>& boundMin = inBounds.getBaseMin();
        const Vec<3, float>& boundMax = inBounds.getBaseMax();

        if (m_min.x < boundMin.x || m_min.y < boundMin.y || m_min.z < boundMin.z)
        {
            m_baseMin = boundMin;
        }

        if (m_max.x > boundMax.x || m_max.y > boundMax.y || m_max.z > boundMax.z)
        {
            m_baseMax = boundMax;
        }

        refresh();
    }

    void Bounds::update(const Transform::Combined& inTransform)
    {
        const Vec<3, float>& scale       = inTransform.getScale();
        const Vec<3, float>& translation = inTransform.getTranslation();

        m_min    = (m_baseMin * scale) + translation;
        m_max    = (m_baseMax * scale) + translation;
        m_top    = (m_baseTop * scale) + translation;
        m_center = (m_baseCenter * scale) + translation;
        m_bottom = (m_baseBottom * scale) + translation;
    }

    const Vec<3, float>& Bounds::getMin() const
    {
        return m_min;
    }

    const Vec<3, float>& Bounds::getBaseMin() const
    {
        return m_baseMin;
    }

    const Vec<3, float>& Bounds::getMax() const
    {
        return m_max;
    }

    const Vec<3, float>& Bounds::getBaseMax() const
    {
        return m_baseMax;
    }

    const Vec<3, float>& Bounds::getBottom() const
    {
        return m_bottom;
    }

    const Vec<3, float>& Bounds::getBaseBottom() const
    {
        return m_baseBottom;
    }

    const Vec<3, float>& Bounds::getTop() const
    {
        return m_top;
    }

    const Vec<3, float>& Bounds::getBaseCenter() const
    {
        return m_baseCenter;
    }

    const Vec<3, float>& Bounds::getCenter() const
    {
        return m_center;
    }

    const Vec<3, float>& Bounds::getBaseTop() const
    {
        return m_baseCenter;
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
        m_bottom = m_baseBottom;
        m_center = m_baseCenter;
        m_top    = m_baseTop;
    }
}