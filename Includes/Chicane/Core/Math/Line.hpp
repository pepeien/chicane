#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Line
    {
    public:
        bool isEmpty() const;

        const Vec2& getStartPoint() const;
        const Vec2& getEndPoint() const;

        const std::vector<Vec2>& getPoints() const;
        void addPoint(const Vec2& inPoint);

    protected:
        std::vector<Vec2> m_points;
    };
}