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
        return Time::miliseconds(inTime) * 0.001f;
    }

    float Time::minutes(Clock::duration inTime)
    {
        return Time::seconds(inTime) / 60.0f;
    }

    float Time::hours(Clock::duration inTime)
    {
        return Time::minutes(inTime) / 60.0f;
    }

    Time Time::fromMilliseconds(float inValue)
    {
        return Time(
            std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<float, std::milli>(inValue < 0.0f ? 0.0f : inValue)
            )
        );
    }

    Time Time::fromSeconds(float inValue)
    {
        return Time::fromMilliseconds(inValue * 1000.0f);
    }

    Time Time::fromMinutes(float inValue)
    {
        return Time::fromSeconds(inValue * 60.0f);
    }

    Time Time::fromHours(float inValue)
    {
        return Time::fromMinutes(inValue * 60.0f);
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