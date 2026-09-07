#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE Contour
    {
    public:
        using Point  = std::array<float, 2>;
        using Points = std::vector<Point>;

    public:
        static constexpr float       MIN_LENGTH               = 1.0e-12f;
        static constexpr float       SNAP_RELATIVE            = 1.0e-6f;
        static constexpr float       CONVEX_AREA_RELATIVE     = 1.0e-8f;
        static constexpr float       DEGENERATE_AREA_RELATIVE = 1.0e-12f;
        static constexpr float       TESS_EXTENT              = 1024.0f;
        static constexpr std::size_t MIN_POLYGON_VERTICES     = 3;
        static constexpr std::size_t MAX_FAN_VERTICES         = 4;
        static constexpr int         TESS_POLYGON_SIZE        = 3;
        static constexpr int         TESS_VERTEX_SIZE         = 2;
        static constexpr float       TESS_NORMAL[3]           = {0.0f, 0.0f, 1.0f};

    public:
        Contour();

    public:
        void triangulate(const Curve::List& inCurves, bool bInIsEvenOdd = true);

        const Vertex::Positions& getPositions() const;
        const Vertex::Indices& getIndices() const;

    private:
        Vertex::Positions m_positions;
        Vertex::Indices   m_indices;
    };
}