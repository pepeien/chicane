#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Bounds/3D/Corner.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
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

        const Vec3& getTop() const;
        const Vec3& getCenter() const;
        const Vec3& getBottom() const;

        const Vec3& getSize() const;

        const Bounds3DCorner& getMin() const;
        const Bounds3DCorner& getMax() const;

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

        Bounds3DCorner  m_min;
        Bounds3DCorner  m_max;

        Vertex::List    m_corners;
        Vertex::List    m_baseCorners;

        Vertex::Indices m_indices;
    };
}