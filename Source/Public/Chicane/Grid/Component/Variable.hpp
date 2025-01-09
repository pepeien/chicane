#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            typedef std::any* Variable;
            typedef std::unordered_map<std::string, Variable> Variables;
        }
    }
}