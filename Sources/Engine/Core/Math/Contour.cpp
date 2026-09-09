#include "Chicane/Core/Math/Contour.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

#include <tesselator.h>

namespace Chicane
{
    static float signedArea(const Contour::Point& inA, const Contour::Point& inB, const Contour::Point& inC)
    {
        return ((inB[0] - inA[0]) * (inC[1] - inA[1])) - ((inC[0] - inA[0]) * (inB[1] - inA[1]));
    }

    static float contourExtent(const Contour::Points& inPoints)
    {
        if (inPoints.empty())
        {
            return 0.0f;
        }

        Vec2 min(inPoints.front()[0], inPoints.front()[1]);
        Vec2 max = min;

        for (const Contour::Point& point : inPoints)
        {
            min.x = std::min(min.x, point[0]);
            min.y = std::min(min.y, point[1]);
            max.x = std::max(max.x, point[0]);
            max.y = std::max(max.y, point[1]);
        }

        return std::max(max.x - min.x, max.y - min.y);
    }

    static Contour::Points normalizeContour(const Contour::Points& inPoints)
    {
        Contour::Points clean;

        if (inPoints.empty())
        {
            return clean;
        }

        const float extent = contourExtent(inPoints);
        const float snap   = std::max(extent * Contour::SNAP_RELATIVE, Contour::MIN_LENGTH);
        const float snap2  = snap * snap;

        auto tooClose = [snap2](const Contour::Point& inLeft, const Contour::Point& inRight)
        {
            const float dx = inLeft[0] - inRight[0];
            const float dy = inLeft[1] - inRight[1];

            return ((dx * dx) + (dy * dy)) <= snap2;
        };

        for (const Contour::Point& point : inPoints)
        {
            if (!clean.empty() && tooClose(clean.back(), point))
            {
                continue;
            }

            clean.push_back(point);
        }

        if (clean.size() >= 2 && tooClose(clean.front(), clean.back()))
        {
            clean.pop_back();
        }

        return clean;
    }

    static bool isConvexContour(const Contour::Points& inPoints)
    {
        if (inPoints.size() < Contour::MIN_POLYGON_VERTICES)
        {
            return false;
        }

        const float       extent  = contourExtent(inPoints);
        const float       minArea = std::max(extent * extent * Contour::CONVEX_AREA_RELATIVE, Contour::MIN_LENGTH);
        const std::size_t count   = inPoints.size();
        int               sign    = 0;

        for (std::size_t i = 0; i < count; ++i)
        {
            const float area = signedArea(inPoints[i], inPoints[(i + 1) % count], inPoints[(i + 2) % count]);

            if (std::fabs(area) <= minArea)
            {
                continue;
            }

            const int next = area > 0.0f ? 1 : -1;

            if (sign != 0 && next != sign)
            {
                return false;
            }

            sign = next;
        }

        return sign != 0;
    }

    static void emitFan(const Contour::Points& inPoints, Vertex::Positions& outPositions, Vertex::Indices& outIndices)
    {
        const float extent        = contourExtent(inPoints);
        const float minArea       = std::max(extent * extent * Contour::DEGENERATE_AREA_RELATIVE, Contour::MIN_LENGTH);
        const std::uint32_t base  = static_cast<std::uint32_t>(outPositions.size());
        const std::uint32_t count = static_cast<std::uint32_t>(inPoints.size());

        for (const Contour::Point& point : inPoints)
        {
            outPositions.push_back({point[0], point[1], 0.0f});
        }

        for (std::uint32_t i = 1; i + 1 < count; ++i)
        {
            if (std::fabs(signedArea(inPoints[0], inPoints[i], inPoints[i + 1])) <= minArea)
            {
                continue;
            }

            outIndices.push_back(base);
            outIndices.push_back(base + i);
            outIndices.push_back(base + i + 1);
        }
    }

    Contour::Contour()
        : m_positions({}),
          m_indices({})
    {}

    void Contour::triangulate(const Curve::List& inCurves, bool bInIsEvenOdd)
    {
        m_positions.clear();
        m_indices.clear();

        if (inCurves.empty())
        {
            return;
        }

        std::vector<Points> contours;

        Vec2 min        = Vec2::Zero();
        Vec2 max        = Vec2::Zero();
        bool bHasBounds = false;

        for (const Curve& curve : inCurves)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            Points points;

            for (const Vec2& point : curve.getPoints())
            {
                points.push_back({point.x, point.y});
            }

            points = normalizeContour(points);

            if (points.size() < MIN_POLYGON_VERTICES)
            {
                continue;
            }

            for (const Point& point : points)
            {
                if (!bHasBounds)
                {
                    min        = Vec2(point[0], point[1]);
                    max        = min;
                    bHasBounds = true;

                    continue;
                }

                min.x = std::min(min.x, point[0]);
                min.y = std::min(min.y, point[1]);
                max.x = std::max(max.x, point[0]);
                max.y = std::max(max.y, point[1]);
            }

            contours.push_back(std::move(points));
        }

        if (contours.empty() || !bHasBounds)
        {
            return;
        }

        if (contours.size() == 1 && contours.front().size() <= MAX_FAN_VERTICES && isConvexContour(contours.front()))
        {
            emitFan(contours.front(), m_positions, m_indices);

            return;
        }

        const float extent = std::max(max.x - min.x, max.y - min.y);

        if (extent <= MIN_LENGTH)
        {
            return;
        }

        const float scale = TESS_EXTENT / extent;

        TESStesselator* tess = tessNewTess(nullptr);

        if (!tess)
        {
            return;
        }

        for (const Points& contour : contours)
        {
            std::vector<float> flat;
            flat.reserve(contour.size() * 2);

            for (const Point& point : contour)
            {
                flat.push_back((point[0] - min.x) * scale);
                flat.push_back((point[1] - min.y) * scale);
            }

            tessAddContour(
                tess,
                TESS_VERTEX_SIZE,
                flat.data(),
                static_cast<int>(sizeof(float) * TESS_VERTEX_SIZE),
                static_cast<int>(contour.size())
            );
        }

        const int winding = bInIsEvenOdd ? TESS_WINDING_ODD : TESS_WINDING_NONZERO;

        if (!tessTesselate(tess, winding, TESS_POLYGONS, TESS_POLYGON_SIZE, TESS_VERTEX_SIZE, TESS_NORMAL))
        {
            tessDeleteTess(tess);

            if (contours.size() == 1 && contours.front().size() <= MAX_FAN_VERTICES)
            {
                emitFan(contours.front(), m_positions, m_indices);
            }

            return;
        }

        const float* verts   = tessGetVertices(tess);
        const int*   indices = tessGetElements(tess);
        const int    nverts  = tessGetVertexCount(tess);
        const int    nelems  = tessGetElementCount(tess);
        const float  inv     = 1.0f / scale;
        const float  minArea = std::max(extent * extent * DEGENERATE_AREA_RELATIVE, MIN_LENGTH);

        for (int i = 0; i < nverts; ++i)
        {
            m_positions.push_back({(verts[i * 2] * inv) + min.x, (verts[i * 2 + 1] * inv) + min.y, 0.0f});
        }

        for (int i = 0; i < nelems; ++i)
        {
            const int* tri = &indices[i * 3];

            if (tri[0] == TESS_UNDEF || tri[1] == TESS_UNDEF || tri[2] == TESS_UNDEF)
            {
                continue;
            }

            const Vertex::Position& a = m_positions.at(static_cast<std::size_t>(tri[0]));
            const Vertex::Position& b = m_positions.at(static_cast<std::size_t>(tri[1]));
            const Vertex::Position& c = m_positions.at(static_cast<std::size_t>(tri[2]));

            if (std::fabs(signedArea({a.x, a.y}, {b.x, b.y}, {c.x, c.y})) <= minArea)
            {
                continue;
            }

            m_indices.push_back(static_cast<std::uint32_t>(tri[0]));
            m_indices.push_back(static_cast<std::uint32_t>(tri[1]));
            m_indices.push_back(static_cast<std::uint32_t>(tri[2]));
        }

        tessDeleteTess(tess);
    }

    const Vertex::Positions& Contour::getPositions() const
    {
        return m_positions;
    }

    const Vertex::Indices& Contour::getIndices() const
    {
        return m_indices;
    }
}