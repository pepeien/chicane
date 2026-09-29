#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Script
    {
        class CHICANE_CORE Handle
        {
        public:
            static void add(const void* inValue);
            static void remove(const void* inValue);
            static bool contains(const void* inValue);
        };
    }
}
