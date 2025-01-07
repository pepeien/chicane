#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Renderer/Vertex/Instance.hpp"

namespace Chicane
{
    namespace Vertex
    {
        Vec<3, float> generateExtent(const std::vector<Vertex::Instance>& inVertices);
    }
}