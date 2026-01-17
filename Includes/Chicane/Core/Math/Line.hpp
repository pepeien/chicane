#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    struct CHICANE_CORE Line
    {
    public:
        bool isEmpty() const;

        Vec2 getStartPoint() const;
        Vec2 getEndPoint() const;

        const std::vector<Vec2>& getPoints() const;
        void addPoint(const Vec2& inPoint);

    protected:
        std::vector<Vec2> m_points;
    };
}