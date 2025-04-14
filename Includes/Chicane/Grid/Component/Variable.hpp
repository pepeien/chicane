#pragma once

#include "Chicane.hpp"
#include "Chicane/Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            typedef std::unordered_map<std::string, Reference*> Variables;
        }
    }
}