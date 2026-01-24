#pragma once

#include "Chicane/Core/Reference.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID FunctionData
        {
        public:
            String                 name   = "";
            std::vector<Reference> params = {};
        };
    }
}