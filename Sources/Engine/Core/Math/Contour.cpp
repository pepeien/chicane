#include "Chicane/Core/Math/Contour.hpp"

#include <tesselator.h>

namespace Chicane
{
    std::vector<std::array<float, 2>> normalize(const std::vector<std::array<float, 2>>& inPoints)
    {
        std::vector<std::array<float, 2>> clean;

        if (inPoints.empty())
        {
            return clean;
        }

        clean.push_back(inPoints[0]);

        for (size_t i = 1; i < inPoints.size(); ++i)
        {
            if (inPoints[i] != inPoints[i - 1])
            {
                clean.push_back(inPoints[i]);
            }
        }

        if (clean.front() == clean.back())
        {
            clean.pop_back();
        }

        return clean;
    }

    Contour::Contour()
        : m_vertices({}),
          m_indices({})
    {}

    void Contour::triangulate(const Curve::List& inCurves)
    {
        m_vertices.clear();
        m_indices.clear();

        if (inCurves.empty())
        {
            return;
        }

        TESStesselator* tess = tessNewTess(nullptr);

        if (!tess)
        {
            return;
        }

        for (const Curve& curve : inCurves)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            std::vector<std::array<float, 2>> points;

            for (const Vec2& point : curve.getPoints())
            {
                points.push_back({point.x, point.y});
            }

            points = normalize(points);

            if (points.size() < 3)
            {
                continue;
            }

            std::vector<float> flat;
            flat.reserve(points.size() * 2);

            for (const auto& p : points)
            {
                flat.push_back(p[0]);
                flat.push_back(p[1]);
            }

            tessAddContour(tess, 2, flat.data(), sizeof(float) * 2, static_cast<int>(points.size()));
        }

        if (!tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, nullptr))
        {
            tessDeleteTess(tess);

            return;
        }

        const float* verts   = tessGetVertices(tess);
        const int*   indices = tessGetElements(tess);
        const int    nverts  = tessGetVertexCount(tess);
        const int    nelems  = tessGetElementCount(tess);

        for (int i = 0; i < nverts; ++i)
        {
            m_vertices.push_back({verts[i * 2], verts[i * 2 + 1], 0.0f});
        }

        for (int i = 0; i < nelems; ++i)
        {
            const int* tri = &indices[i * 3];

            if (tri[0] == TESS_UNDEF || tri[1] == TESS_UNDEF || tri[2] == TESS_UNDEF)
            {
                continue;
            }

            m_indices.push_back(static_cast<std::uint32_t>(tri[0]));
            m_indices.push_back(static_cast<std::uint32_t>(tri[1]));
            m_indices.push_back(static_cast<std::uint32_t>(tri[2]));
        }

        tessDeleteTess(tess);
    }

    const Contour::Vertices& Contour::getVertices() const
    {
        return m_vertices;
    }

    const Contour::Indices& Contour::getIndices() const
    {
        return m_indices;
    }
}