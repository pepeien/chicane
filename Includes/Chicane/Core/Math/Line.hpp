#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    struct CHICANE_CORE Line
    {
    public:
        using Points = std::vector<Vec2>;

    public:
        bool isEmpty() const;

        Vec2 getStartPoint() const;
        Vec2 getEndPoint() const;

        const Points& getPoints() const;
        void addPoint(const Vec2& inPoint);

    protected:
        Points m_points;
    };
}