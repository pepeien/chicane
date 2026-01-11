#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE BoundsPoints
    {
    public:
        BoundsPoints(const std::vector<Vertex>& inVertices);
        BoundsPoints();

    public:
        void add(const Vec3& inMin, const Vec3& inMax);

        const Vec3& getMin() const;
        void setMin(const Vec3& inValue);

        const Vec3& getMax() const;
        void setMax(const Vec3& inValue);

        const Vec3& getTop() const;
        void setTop(const Vec3& inValue);

        const Vec3& getCenter() const;
        void setCenter(const Vec3& inValue);

        const Vec3& getBottom() const;
        void setBottom(const Vec3& inValue);

    private:
        Vec3 m_min;
        Vec3 m_max;
        Vec3 m_top;
        Vec3 m_center;
        Vec3 m_bottom;
    };
}