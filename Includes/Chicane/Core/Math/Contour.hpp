#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Contour
    {
    public:
        using Vertices = std::vector<Vec3>;
        using Indices  = std::vector<std::uint32_t>;

    public:
        Contour();

    public:
        void triangulate(const Curve::List& inCurves);

        const Vertices& getVertices() const;
        const Indices& getIndices() const;

    private:
        Vertices m_vertices;
        Indices  m_indices;
    };
}