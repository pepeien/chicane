#pragma once

#include <vector>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        struct CHICANE_GRID Projection
        {
        public:
            explicit Projection(std::vector<Component*>& inChildren);

            ~Projection();

            static std::vector<Component*>* sCurrent();

        public:
            std::vector<Component*>* previous = nullptr;
        };
    }
}
