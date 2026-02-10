#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    struct CHICANE_CORE Timer
    {
    public:
        Timer();

    protected:
        inline virtual void onTime() { return; }

    public:
        void start();
        void end();

        Time getDelta() const;

    protected:
        Time m_start;
        Time m_end;
    };
}