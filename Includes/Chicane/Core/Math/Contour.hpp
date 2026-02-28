#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE Contour
    {
    public:
        Contour();

    public:
        void triangulate(const Curve::List& inCurves);

        const Vertex::Positions& getPositions() const;
        const Vertex::Indices& getIndices() const;

    private:
        Vertex::Positions m_positions;
        Vertex::Indices   m_indices;
    };
}