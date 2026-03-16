#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionEnumeratorInfo
    {
    public:
        String name  = "";
        int    value = 0;
    };
}