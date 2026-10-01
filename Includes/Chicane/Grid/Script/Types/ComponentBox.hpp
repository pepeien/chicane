#pragma once

#include "Chicane/Grid.hpp"

namespace Chicane
{
    struct ReflectionTypeInfo;

    namespace Grid
    {
        class Component;

        namespace Types
        {
            struct CHICANE_GRID ComponentBox
            {
            public:
                void*                     instance = nullptr;
                const ReflectionTypeInfo* type     = nullptr;
                Component*                owner    = nullptr;
            };
        }
    }
}
