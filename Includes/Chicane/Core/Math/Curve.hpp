#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    struct CHICANE_CORE Curve : public Line
    {
    public:
        void setSegmentCount(std::uint32_t inSegmentCount);

        void addQuadraticPoint(
            const Vec<2, float>& inControl,
            const Vec<2, float>& inPoint
        );
        void addBezierPoint(
            const Vec<2, float>& inControlA,
            const Vec<2, float>& inControlB,
            const Vec<2, float>& inPoint
        );

    protected:
        std::uint32_t m_segmentCount = 0;
    };
}