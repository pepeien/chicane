#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds3D
    {
    public:
        Bounds3D(const Vertex::List& inVertices, const Vertex::Indices& inIndices);
        Bounds3D();

    public:
        void set(const Bounds3D& inBounds);
        void add(const Bounds3D& inBounds);

        void update(const SpatialTransform& inTransform);

        bool intersects(const Bounds3D& inBounds) const;
        bool intersects(const Vec3& inPoint) const;

        Vec3 getOverlap(const Bounds3D& inBounds) const;

        const Vec3& getMin() const;
        const Vec3& getBaseMin() const;

        const Vec3& getMax() const;
        const Vec3& getBaseMax() const;

        const Vec3& getCenter() const;
        const Vec3& getBaseCenter() const;

        const Vec3& getTop() const;
        const Vec3& getBaseTop() const;

        const Vec3& getBottom() const;
        const Vec3& getBaseBottom() const;

        const std::vector<Vec3>& getVertices() const;
        const Vertex::Indices& getIndices() const;

    private:
        void refresh();

    private:
        Vec3              m_min;
        Vec3              m_baseMin;

        Vec3              m_max;
        Vec3              m_baseMax;

        Vec3              m_top;
        Vec3              m_baseTop;

        Vec3              m_center;
        Vec3              m_baseCenter;

        Vec3              m_bottom;
        Vec3              m_baseBottom;

        std::vector<Vec3> m_vertices;
        Vertex::Indices   m_indices;
    };
}