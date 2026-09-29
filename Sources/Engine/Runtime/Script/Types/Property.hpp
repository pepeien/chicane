#pragma once

#include "Chicane/Runtime/Script/Types.hpp"

namespace Chicane
{
    struct ReflectionTypeMethodInfo;

    namespace Types
    {
        struct MethodBox
        {
            Object*                         instance;
            const ReflectionTypeMethodInfo* method;
        };
    }
}
