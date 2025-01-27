#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Renderer/Vertex/Instance.hpp"

namespace Chicane
{
    struct Bounds
    {
    public:
        Bounds(const std::vector<Vertex::Instance>& inVertices);
        Bounds() = default;

    public:
        void update(const Transform::Combined& inTransform);

        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec<3, float>& inPoint) const;

        const Vec<3, float>& getMin() const;
        const Vec<3, float>& getMax() const;

        const Vec<3, float>& getTop() const;
        const Vec<3, float>& getCenter() const;
        const Vec<3, float>& getOrigin() const;

    private:
        Vec<3, float>       m_min           = Vec3Zero;
        Vec<3, float>       m_currentMin    = Vec3Zero;

        Vec<3, float>       m_max           = Vec3Zero;
        Vec<3, float>       m_currentMax    = Vec3Zero;

        Vec<3, float>       m_top           = Vec3Zero;
        Vec<3, float>       m_currentTop    = Vec3Zero;

        Vec<3, float>       m_origin        = Vec3Zero;
        Vec<3, float>       m_currentOrigin = Vec3Zero;

        Vec<3, float>       m_center        = Vec3Zero;
        Vec<3, float>       m_currentCenter = Vec3Zero;
    };
}