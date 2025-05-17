#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    struct CHICANE_CORE Curve : public Line
    {
    public:
        static std::vector<Vec<3, float>> getTriangleVertices(
            const std::vector<Curve>& inContours,
            float inPixelSize = 1.0f
        );
        static std::vector<std::uint32_t> getTriangleIndices(const std::vector<Curve>& inContours);

    public:
        bool isClosuredWith(const Curve& inCurve) const;

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

        float getSignedArea() const;

    protected:
        std::uint32_t m_segmentCount = 0;
    };
}