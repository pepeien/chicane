#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    bool Line::isEmpty() const
    {
        return m_points.empty();
    }

    const Vec<2, float>& Line::getStartPoint() const
    {
        if (isEmpty())
        {
            return Vec2Zero;
        }

        return m_points.front();
    }

    const Vec<2, float>& Line::getEndPoint() const
    {
        if (isEmpty())
        {
            return Vec2Zero;
        }

        return m_points.back();
    }

    const std::vector<Vec<2, float>>& Line::getPoints() const
    {
        return m_points;
    }

    void Line::addPoint(const Vec<2, float>& inPoint)
    {
        m_points.push_back(inPoint);
    }
}