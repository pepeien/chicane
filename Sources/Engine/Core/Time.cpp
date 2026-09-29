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

    float Time::sMiliseconds(Clock::duration inDuration)
    {
        return std::chrono::duration<float, std::milli>(inDuration).count();
    }

    float Time::sSeconds(Clock::duration inTime)
    {
        return Time::sMiliseconds(inTime) * 0.001f;
    }

    float Time::sMinutes(Clock::duration inTime)
    {
        return Time::sSeconds(inTime) / 60.0f;
    }

    float Time::sHours(Clock::duration inTime)
    {
        return Time::sMinutes(inTime) / 60.0f;
    }

    Time Time::sFromMilliseconds(float inValue)
    {
        return Time(
            std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<float, std::milli>(inValue < 0.0f ? 0.0f : inValue)
            )
        );
    }

    Time Time::sFromSeconds(float inValue)
    {
        return Time::sFromMilliseconds(inValue * 1000.0f);
    }

    Time Time::sFromMinutes(float inValue)
    {
        return Time::sFromSeconds(inValue * 60.0f);
    }

    Time Time::sFromHours(float inValue)
    {
        return Time::sFromMinutes(inValue * 60.0f);
    }

    float Time::miliseconds() const
    {
        return Time::sMiliseconds(point.time_since_epoch());
    }

    float Time::seconds() const
    {
        return Time::sSeconds(point.time_since_epoch());
    }

    float Time::minutes() const
    {
        return Time::sMinutes(point.time_since_epoch());
    }

    float Time::hours() const
    {
        return Time::sHours(point.time_since_epoch());
    }
}