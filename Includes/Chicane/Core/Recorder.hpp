#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    struct CHICANE_CORE Recorder
    {
    public:
        Recorder(float inUpdateRate);
        Recorder();

    protected:
        virtual void onTime() { return; }

    public:
        void start();
        void end();

        void setUpdateRate(float inValue);

    protected:
        Time  m_begin;
        Time  m_end;

        float m_cooldown;
    };
}