#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    bool Line::isEmpty() const
    {
        return m_points.empty();
    }

    Vec2 Line::getStartPoint() const
    {
        if (isEmpty())
        {
            return Vec2::Zero;
        }

        return m_points.front();
    }

    Vec2 Line::getEndPoint() const
    {
        if (isEmpty())
        {
            return Vec2::Zero;
        }

        return m_points.back();
    }

    const Line::Points& Line::getPoints() const
    {
        return m_points;
    }

    void Line::addPoint(const Vec2& inPoint)
    {
        m_points.push_back(inPoint);
    }
}