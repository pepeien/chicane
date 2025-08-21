#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Transform/Spatial.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds
    {
    public:
        Bounds(const std::vector<Vertex>& inVertices);
        Bounds() = default;

    public:
        void set(const Bounds& inBounds);
        void add(const Bounds& inBounds);

        void update(const SpatialTransform& inTransform);

        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec3& inPoint) const;

        Vec3 getOverlap(const Bounds& inBounds) const;

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
        const std::vector<std::uint32_t>& getIndices() const;

    private:
        void refresh();

    private:
        Vec3                       m_min        = Vec3::Zero;
        Vec3                       m_baseMin    = Vec3::Zero;

        Vec3                       m_max        = Vec3::Zero;
        Vec3                       m_baseMax    = Vec3::Zero;

        Vec3                       m_top        = Vec3::Zero;
        Vec3                       m_baseTop    = Vec3::Zero;

        Vec3                       m_center     = Vec3::Zero;
        Vec3                       m_baseCenter = Vec3::Zero;

        Vec3                       m_bottom     = Vec3::Zero;
        Vec3                       m_baseBottom = Vec3::Zero;

        std::vector<Vec3>          m_vertices   = {};
        std::vector<std::uint32_t> m_indices    = {};
    };
}