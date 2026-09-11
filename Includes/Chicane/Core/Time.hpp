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
        static inline Time Zero() { return Time(Clock::duration::zero()); }

        static float miliseconds(Clock::duration inDuration);
        static float seconds(Clock::duration inTime);
        static float minutes(Clock::duration inTime);
        static float hours(Clock::duration inTime);

        static Time fromMilliseconds(float inValue);
        static Time fromSeconds(float inValue);
        static Time fromMinutes(float inValue);
        static Time fromHours(float inValue);

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
            return Time::fromMilliseconds(inTime.miliseconds() * inScale);
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
