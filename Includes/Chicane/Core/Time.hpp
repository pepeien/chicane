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
        Time(const Clock::duration& inDuration);
        Time(const Point& inDuration);
        Time();

    public:
        static float miliseconds(Clock::duration inDuration);
        static float seconds(Clock::duration inTime);
        static float minutes(Clock::duration inTime);
        static float hours(Clock::duration inTime);

    public:
        float miliseconds() const;
        float seconds() const;
        float minutes() const;
        float hours() const;

    public:
        Point point;
    };
}