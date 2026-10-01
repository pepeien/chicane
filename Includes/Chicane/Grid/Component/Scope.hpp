#pragma once

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        struct CHICANE_GRID Scope
        {
        public:
            explicit Scope(Component* inComponent);

            ~Scope();

            static Component* sCurrent();

        public:
            Component* previous = nullptr;
        };
    }
}
