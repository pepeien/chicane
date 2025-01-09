#pragma once

#include "Chicane/Game/Controller.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{
    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
        const std::vector<Actor*>& inIgnoredActors
    );
}