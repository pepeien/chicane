#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Actor;
    class Object;

    CH_TYPE(Type = (Manual), Alias = (Trace))
    struct CHICANE_RUNTIME SceneTraceResponse
    {
    public:
        CH_FUNCTION()
        const Vec3& getLocation() const { return location; }

        CH_FUNCTION()
        const Vec3& getImpact() const { return impact; }

        CH_FUNCTION()
        float getDistance() const { return distance; }

        CH_FUNCTION()
        const Vec3& getStart() const { return start; }

        CH_FUNCTION()
        const Vec3& getEnd() const { return end; }

        CH_FUNCTION()
        Object* getObject() const;

        CH_FUNCTION()
        Actor* getActor() const;

    public:
        CH_FIELD()
        Vec3 location = Vec3::sZero();

        CH_FIELD()
        Vec3 impact = Vec3::sZero();

        CH_FIELD()
        float distance = 0.0f;

        CH_FIELD()
        Vec3 start = Vec3::sZero();

        CH_FIELD()
        Vec3 end = Vec3::sZero();

        CH_FIELD()
        Object* object = nullptr;

    public:
        Time::Point timestamp = {};
    };
}
