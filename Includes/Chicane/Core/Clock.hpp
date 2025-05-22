#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    using Clock = std::chrono::steady_clock;

    struct CHICANE_CORE Time
    {
    public:
        using Point = std::chrono::time_point<Clock>;

    public:
        Time()
            : point(Clock::now())
        {}

        Time(const std::chrono::time_point<Clock>& inDuration)
            : point(inDuration)
        {}

    public:
        static float miliseconds(Clock::duration inDuration)
        {
            return std::chrono::duration<float, std::milli>(inDuration).count();
        }

        static float seconds(Clock::duration inTime)
        {
            return Time::miliseconds(inTime) * 1000;
        }

        static float minutes(Clock::duration inTime)
        {
            return Time::seconds(inTime) * 60;
        }

        static float hours(Clock::duration inTime)
        {
            return Time::minutes(inTime) * 60;
        }

        float miliseconds() const
        {
            return Time::miliseconds(point.time_since_epoch());
        }

        float seconds() const
        {
            return Time::seconds(point.time_since_epoch());
        }

        float minutes() const
        {
            return Time::minutes(point.time_since_epoch());
        }

        float hours() const
        {
            return Time::hours(point.time_since_epoch());
        }
    
    public:
        Point point;
    };
}