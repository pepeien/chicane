#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Bounds/Corner.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds3D
    {
    public:
        Bounds3D(const Vertex::List& inVertices);
        Bounds3D();

    public:
        bool intersects(const Bounds3D& inOther) const;
        bool intersects(const Vec3& inPoint) const;

        Vec3 getOverlap(const Bounds3D& inBounds) const;

        const Vec3& getTop() const;
        const Vec3& getCenter() const;
        const Vec3& getBottom() const;

        const Vec3& getSize() const;

        const BoundsCorner& getMin() const;
        const BoundsCorner& getMax() const;

        const Vertex::List& getCorners() const;
        const Vertex::Indices& getIndices() const;

        void add(const Bounds3D& inBounds);
        void add(const Vertex::List& inVertices);

        void transform(const Mat4& inModel);

    private:
        void refreshCorners(Vertex::List& outValue, const Vec3& inMin, const Vec3& inMax) const;

    private:
        Vec3            m_top;
        Vec3            m_center;
        Vec3            m_bottom;

        Vec3            m_size;

        BoundsCorner    m_min;
        BoundsCorner    m_max;

        Vertex::List    m_corners;
        Vertex::List    m_baseCorners;

        Vertex::Indices m_indices;
    };
}