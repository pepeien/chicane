#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Enumerator/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionEnumInfo
    {
    public:
        using Enumerators = std::vector<ReflectionEnumeratorInfo>;

    public:
        String      name        = "";
        Enumerators enumerators = {};
    };
}