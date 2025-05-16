#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Line
    {
    public:
        bool isEmpty() const;

        const Vec<2, float>& getStartPoint() const;
        const Vec<2, float>& getEndPoint() const;

        const std::vector<Vec<2, float>>& getPoints() const;
        void addPoint(const Vec<2, float>& inPoint);

    protected:
        std::vector<Vec<2, float>> m_points;
    };
}