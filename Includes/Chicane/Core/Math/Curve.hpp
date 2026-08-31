#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    struct CHICANE_CORE Curve : public Line
    {
    public:
        using List = std::vector<Curve>;

        static constexpr inline float         FLATNESS  = 0.35f;
        static constexpr inline std::uint32_t MAX_DEPTH = 8;

    public:
        void setSegmentCount(std::uint32_t inSegmentCount);

        void addQuadraticPoint(const Vec2& inControl, const Vec2& inPoint);
        void addBezierPoint(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint);

    protected:
        std::uint32_t m_segmentCount = 0;
    };
}