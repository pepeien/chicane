#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Renderer/Vertex/Instance.hpp"

namespace Chicane
{
    struct Bounds
    {
    public:
        static Vec<3, float> extentFromVertices(const std::vector<Vertex::Instance>& inVertices);

    public:
        Bounds(const std::vector<Vertex::Instance>& inVertices);
        Bounds() = default;

    public:
        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec<3, float>& inPoint) const;

    public:
        Vec<3, float> extent = Vec3Zero;

        Vec<3, float> top    = Vec3Zero;
        Vec<3, float> origin = Vec3Zero;
        Vec<3, float> center = Vec3Zero;
    };
}