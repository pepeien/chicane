#pragma once

#include <chrono>

#include "Chicane/Core.hpp"

namespace Chicane
{
    struct CHICANE_CORE Time
    {
    public:
        using Clock = std::chrono::steady_clock;
        using Point = std::chrono::time_point<Clock>;

    public:
        static inline Time sZero() { return Time(Clock::duration::zero()); }

        static float sMiliseconds(Clock::duration inDuration);
        static float sSeconds(Clock::duration inTime);
        static float sMinutes(Clock::duration inTime);
        static float sHours(Clock::duration inTime);

        static Time sFromMilliseconds(float inValue);
        static Time sFromSeconds(float inValue);
        static Time sFromMinutes(float inValue);
        static Time sFromHours(float inValue);

    public:
        Time(const Clock::duration& inDuration);
        Time(const Point& inDuration);
        Time();

    public:
        friend inline bool operator==(const Time& inLeft, const Time& inRight) { return inLeft.point == inRight.point; }

        friend inline bool operator!=(const Time& inLeft, const Time& inRight) { return inLeft.point != inRight.point; }

        friend inline bool operator<(const Time& inLeft, const Time& inRight) { return inLeft.point < inRight.point; }

        friend inline bool operator<=(const Time& inLeft, const Time& inRight) { return inLeft.point <= inRight.point; }

        friend inline bool operator>(const Time& inLeft, const Time& inRight) { return inLeft.point > inRight.point; }

        friend inline bool operator>=(const Time& inLeft, const Time& inRight) { return inLeft.point >= inRight.point; }

        friend inline Time operator+(const Time& inLeft, const Time& inRight)
        {
            return Time(inLeft.point.time_since_epoch() + inRight.point.time_since_epoch());
        }

        friend inline Time operator-(const Time& inLeft, const Time& inRight)
        {
            return Time(inLeft.point - inRight.point);
        }

        friend inline Time operator*(const Time& inTime, float inScale)
        {
            return Time::sFromMilliseconds(inTime.miliseconds() * inScale);
        }

        friend inline Time operator*(float inScale, const Time& inTime) { return inTime * inScale; }

    public:
        float miliseconds() const;
        float seconds() const;
        float minutes() const;
        float hours() const;

    public:
        Point point;
    };
}
