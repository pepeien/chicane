#pragma once

#include <memory>
#include <unordered_map>

#include "Chicane/Core/String.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Queue.hpp"

namespace Chicane
{
    class Transformable;

    namespace Drift
    {
        struct Engine
        {
            std::unordered_map<String, Clip>                           clips;
            std::unordered_map<Transformable*, std::unique_ptr<Queue>> bindings;
        };
    }
}
