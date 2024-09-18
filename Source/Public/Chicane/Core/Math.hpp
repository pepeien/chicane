#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    class Actor;

    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Vec : glm::vec<O, T, Q>
    {
        template<typename... A>
        Vec(A ...args)
            : glm::vec<O, T, Q>(args...)
        {}

        T dot(const Vec<O, T, Q>& inTarget) const
        {
            return glm::dot<O, T, Q>(*this, inTarget);
        }
    };

    
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Mat : glm::mat<O, O, T, Q>
    {
        template<typename... A>
        Mat(A ...args)
            : glm::mat<O, O, T, Q>(args...)
        {}
    };

    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct Quat : glm::qua<T, Q>
    {
        template<typename... A>
        Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };

    const Vec<2, float> Vec2Zero = Vec<2, float>(0.0f);
    const Vec<3, float> Vec3Zero = Vec<3, float>(0.0f);

    struct Transform
    {
        Vec<3, float> translation = Vec3Zero;
        Vec<3, float> rotation    = Vec3Zero;
        Vec<3, float> scale       = Vec<3, float>(1.0f);
    };

    struct Direction
    {
        Vec<3, float> forward = FORWARD_DIRECTION;
        Vec<3, float> up      = UP_DIRECTION;
        Vec<3, float> left    = LEFT_DIRECTION;
    };

    struct Bounds
    {
    public:
        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec<3, float>& inPoint) const;

    public:
        Vec<3, float> extent = Vec3Zero;

        Vec<3, float> top    = Vec3Zero;
        Vec<3, float> origin = Vec3Zero;
        Vec<3, float> center = Vec3Zero;
    };

    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
        const Vec<3, float>& inDirection,
        const std::vector<Actor*>& inIgnoredActors
    );
}