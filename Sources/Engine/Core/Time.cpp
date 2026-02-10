#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    Time::Time(const Clock::duration& inDuration)
        : point(inDuration)
    {}

    Time::Time(const Point& inDuration)
        : point(inDuration)
    {}

    Time::Time()
        : point(Clock::now())
    {}

    float Time::miliseconds(Clock::duration inDuration)
    {
        return std::chrono::duration<float, std::milli>(inDuration).count();
    }

    float Time::seconds(Clock::duration inTime)
    {
        return Time::miliseconds(inTime) * 1000;
    }

    float Time::minutes(Clock::duration inTime)
    {
        return Time::seconds(inTime) * 60;
    }

    float Time::hours(Clock::duration inTime)
    {
        return Time::minutes(inTime) * 60;
    }

    float Time::miliseconds() const
    {
        return Time::miliseconds(point.time_since_epoch());
    }

    float Time::seconds() const
    {
        return Time::seconds(point.time_since_epoch());
    }

    float Time::minutes() const
    {
        return Time::minutes(point.time_since_epoch());
    }

    float Time::hours() const
    {
        return Time::hours(point.time_since_epoch());
    }
}