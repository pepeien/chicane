#pragma once

#include "Chicane.hpp"
#include "Chicane/Box/Asset/Model/Data.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Transform.hpp"

namespace Chicane
{
    struct Bounds
    {
    public:
        Bounds(const std::vector<Box::Model::Vertex>& inVertices);
        Bounds() = default;

    public:
        void set(const Bounds& inBounds);
        void add(const Bounds& inBounds);

        void update(const Transform::Combined& inTransform);

        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec<3, float>& inPoint) const;

        Vec<3, float> getOverlap(const Bounds& inBounds) const;

        const Vec<3, float>& getMin() const;
        const Vec<3, float>& getBaseMin() const;

        const Vec<3, float>& getMax() const;
        const Vec<3, float>& getBaseMax() const;

        const Vec<3, float>& getCenter() const;
        const Vec<3, float>& getBaseCenter() const;

        const Vec<3, float>& getTop() const;
        const Vec<3, float>& getBaseTop() const;

        const Vec<3, float>& getBottom() const;
        const Vec<3, float>& getBaseBottom() const;

        const std::vector<Vec<3, float>>& getCorners() const;

    private:
        void refresh();

    private:
        Vec<3, float>              m_min        = Vec3Zero;
        Vec<3, float>              m_baseMin    = Vec3Zero;

        Vec<3, float>              m_max        = Vec3Zero;
        Vec<3, float>              m_baseMax    = Vec3Zero;

        Vec<3, float>              m_top        = Vec3Zero;
        Vec<3, float>              m_baseTop    = Vec3Zero;

        Vec<3, float>              m_center     = Vec3Zero;
        Vec<3, float>              m_baseCenter = Vec3Zero;

        Vec<3, float>              m_bottom     = Vec3Zero;
        Vec<3, float>              m_baseBottom = Vec3Zero;

        std::vector<Vec<3, float>> m_corners    = {};
    };
}