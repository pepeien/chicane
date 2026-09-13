#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Smoke.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Automatic)
        struct CHICANE_SMOKE Range
        {
        public:
            Range();
            Range(float inValue);
            Range(float inFrom, float inTo);

        public:
            float sample(float inAlpha) const;

        public:
            float from;
            float to;
        };
    }
}
