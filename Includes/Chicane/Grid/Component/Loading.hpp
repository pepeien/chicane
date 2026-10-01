#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Loading
        {
        public:
            Loading(std::vector<String>& inStack, const String& inPath);

            ~Loading();

        public:
            std::vector<String>& stack;
        };
    }
}
