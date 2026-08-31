#include "Chicane/Core/Math/Contour.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

#include <tesselator.h>

namespace Chicane
{
    static constexpr float         kMinLength              = 1.0e-12f;
    static constexpr float         kSnapRelative           = 1.0e-6f;
    static constexpr float         kConvexAreaRelative     = 1.0e-8f;
    static constexpr float         kDegenerateAreaRelative = 1.0e-12f;
    static constexpr float         kTessExtent             = 1024.0f;
    static constexpr std::size_t   kMinPolygonVertices     = 3;
    static constexpr std::size_t   kMaxFanVertices         = 4;
    static constexpr int           kTessPolygonSize        = 3;
    static constexpr int           kTessVertexSize         = 2;
    static constexpr TESSreal      kTessNormal[3]          = {0.0f, 0.0f, 1.0f};

    using ContourPoint  = std::array<float, 2>;
    using ContourPoints = std::vector<ContourPoint>;

    static float signedArea(const ContourPoint& inA, const ContourPoint& inB, const ContourPoint& inC)
    {
        return ((inB[0] - inA[0]) * (inC[1] - inA[1])) - ((inC[0] - inA[0]) * (inB[1] - inA[1]));
    }

    static float contourExtent(const ContourPoints& inPoints)
    {
        if (inPoints.empty())
        {
            return 0.0f;
        }

        float minX = inPoints.front()[0];
        float minY = inPoints.front()[1];
        float maxX = minX;
        float maxY = minY;

        for (const ContourPoint& point : inPoints)
        {
            minX = std::min(minX, point[0]);
            minY = std::min(minY, point[1]);
            maxX = std::max(maxX, point[0]);
            maxY = std::max(maxY, point[1]);
        }

        return std::max(maxX - minX, maxY - minY);
    }

    static ContourPoints normalizeContour(const ContourPoints& inPoints)
    {
        ContourPoints clean;

        if (inPoints.empty())
        {
            return clean;
        }

        const float extent = contourExtent(inPoints);
        const float snap   = std::max(extent * kSnapRelative, kMinLength);
        const float snap2  = snap * snap;

        auto tooClose = [snap2](const ContourPoint& inLeft, const ContourPoint& inRight)
        {
            const float dx = inLeft[0] - inRight[0];
            const float dy = inLeft[1] - inRight[1];

            return ((dx * dx) + (dy * dy)) <= snap2;
        };

        for (const ContourPoint& point : inPoints)
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

    static bool isConvexContour(const ContourPoints& inPoints)
    {
        if (inPoints.size() < kMinPolygonVertices)
        {
            return false;
        }

        const float       extent  = contourExtent(inPoints);
        const float       minArea = std::max(extent * extent * kConvexAreaRelative, kMinLength);
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

    static void emitFan(const ContourPoints& inPoints, Vertex::Positions& outPositions, Vertex::Indices& outIndices)
    {
        const float         extent  = contourExtent(inPoints);
        const float         minArea = std::max(extent * extent * kDegenerateAreaRelative, kMinLength);
        const std::uint32_t base    = static_cast<std::uint32_t>(outPositions.size());
        const std::uint32_t count   = static_cast<std::uint32_t>(inPoints.size());

        for (const ContourPoint& point : inPoints)
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

    void Contour::triangulate(const Curve::List& inCurves, bool bInEvenOdd)
    {
        m_positions.clear();
        m_indices.clear();

        if (inCurves.empty())
        {
            return;
        }

        std::vector<ContourPoints> contours;

        float minX       = 0.0f;
        float minY       = 0.0f;
        float maxX       = 0.0f;
        float maxY       = 0.0f;
        bool  bHasBounds = false;

        for (const Curve& curve : inCurves)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            ContourPoints points;

            for (const Vec2& point : curve.getPoints())
            {
                points.push_back({point.x, point.y});
            }

            points = normalizeContour(points);

            if (points.size() < kMinPolygonVertices)
            {
                continue;
            }

            for (const ContourPoint& point : points)
            {
                if (!bHasBounds)
                {
                    minX       = point[0];
                    minY       = point[1];
                    maxX       = point[0];
                    maxY       = point[1];
                    bHasBounds = true;

                    continue;
                }

                minX = std::min(minX, point[0]);
                minY = std::min(minY, point[1]);
                maxX = std::max(maxX, point[0]);
                maxY = std::max(maxY, point[1]);
            }

            contours.push_back(std::move(points));
        }

        if (contours.empty() || !bHasBounds)
        {
            return;
        }

        if (contours.size() == 1 && contours.front().size() <= kMaxFanVertices && isConvexContour(contours.front()))
        {
            emitFan(contours.front(), m_positions, m_indices);

            return;
        }

        const float extent = std::max(maxX - minX, maxY - minY);

        if (extent <= kMinLength)
        {
            return;
        }

        const float scale = kTessExtent / extent;

        TESStesselator* tess = tessNewTess(nullptr);

        if (!tess)
        {
            return;
        }

        for (const ContourPoints& contour : contours)
        {
            std::vector<float> flat;
            flat.reserve(contour.size() * 2);

            for (const ContourPoint& point : contour)
            {
                flat.push_back((point[0] - minX) * scale);
                flat.push_back((point[1] - minY) * scale);
            }

            tessAddContour(
                tess,
                kTessVertexSize,
                flat.data(),
                static_cast<int>(sizeof(float) * kTessVertexSize),
                static_cast<int>(contour.size())
            );
        }

        const int winding = bInEvenOdd ? TESS_WINDING_ODD : TESS_WINDING_NONZERO;

        if (!tessTesselate(tess, winding, TESS_POLYGONS, kTessPolygonSize, kTessVertexSize, kTessNormal))
        {
            tessDeleteTess(tess);

            if (contours.size() == 1 && contours.front().size() <= kMaxFanVertices)
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
        const float  minArea = std::max(extent * extent * kDegenerateAreaRelative, kMinLength);

        for (int i = 0; i < nverts; ++i)
        {
            m_positions.push_back({(verts[i * 2] * inv) + minX, (verts[i * 2 + 1] * inv) + minY, 0.0f});
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