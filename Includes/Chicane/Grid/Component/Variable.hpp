#pragma once

#include "Core.hpp"
#include "Grid/Reference.hpp"

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