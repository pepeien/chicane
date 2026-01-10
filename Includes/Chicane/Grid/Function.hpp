#pragma once

#include "Chicane/Core/Reference.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        using Function  = std::function<Reference(const Event&)>;
        using Functions = std::unordered_map<String, Function>;
    }
}