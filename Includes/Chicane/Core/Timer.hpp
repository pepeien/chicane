#pragma once

#include "Chicane/Core/Clock.hpp"
#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    struct CHICANE_CORE Timer
    {
    public:
        Timer(float inUpdateRate);
        Timer();

    protected:
        virtual void onTime() { return; }

    public:
        void start();
        void end();

        void setUpdateRate(float inValue);

    protected:
        Time  m_begin;
        Time  m_end;
        Time  m_time;

        float m_cooldown;
    };
}