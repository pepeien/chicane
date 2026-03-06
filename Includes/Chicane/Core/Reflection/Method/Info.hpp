#pragma once

#include <cstddef>
#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionMethodInfo
    {
    public:
        String                     name       = "";
        String                     returnType = "";
        std::vector<String>        paramTypes = {};
        std::function<void(void*)> invoke     = {};
    };
}