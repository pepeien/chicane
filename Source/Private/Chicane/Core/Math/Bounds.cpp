#include "Chicane/Core/Math/Bounds.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;

namespace Chicane
{
    Bounds::Bounds(const std::vector<Vertex::Instance>& inVertices)
    {
        m_min = Vec<3, float>(FLT_MAX);
        m_max = Vec<3, float>(-FLT_MAX);

        for (const Vertex::Instance& vertex : inVertices)
        {
            const Vec<3, float>& position = vertex.position;

            m_min.x = std::min(m_min.x, position.x);
            m_min.y = std::min(m_min.y, position.y);
            m_min.z = std::min(m_min.z, position.z);

            m_max.x = std::max(m_max.x, position.x);
            m_max.y = std::max(m_max.y, position.y);
            m_max.z = std::max(m_max.z, position.z);
        }

        float xCenter = (m_min.x + m_max.x) * 0.5f;
        float yCenter = (m_min.y + m_max.y) * 0.5f;
        float zCenter = m_max.z * 0.5f;

        m_top.x = xCenter;
        m_top.y = yCenter;
        m_top.z = m_max.z;

        m_center.z = zCenter;
        m_center.x = zCenter;
        m_center.y = zCenter;

        m_origin.x = xCenter;
        m_origin.y = yCenter;
        m_origin.z = m_min.z;
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

    void Bounds::update(const Transform::Combined& inTransform)
    {
        const Vec<3, float>& scale       = inTransform.getScale();
        const Vec<3, float>& translation = inTransform.getTranslation();

        m_currentMin    = (m_min * scale) + translation;
        m_currentMax    = (m_max * scale) + translation;
        m_currentTop    = (m_top * scale) + translation;
        m_currentCenter = (m_center * scale) + translation;
        m_currentOrigin = (m_origin * scale) + translation;
    }

    const Vec<3, float>& Bounds::getMin() const
    {
        return m_currentMin;
    }

    const Vec<3, float>& Bounds::getMax() const
    {
        return m_currentMax;
    }

    const Vec<3, float>& Bounds::getTop() const
    {
        return m_currentTop;
    }

    const Vec<3, float>& Bounds::getCenter() const
    {
        return m_currentCenter;
    }

    const Vec<3, float>& Bounds::getOrigin() const
    {
        return m_currentOrigin;
    }
}