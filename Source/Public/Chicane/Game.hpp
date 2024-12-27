#pragma once

#include "Chicane/Game/Controller.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Game/Transformable/Actor.hpp"
#include "Chicane/Game/Transformable/Component.hpp"
#include "Chicane/Game/Transformable/Component/CollisionComponent.hpp"
#include "Chicane/Game/Transformable/Component/CameraComponent.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn/CameraPawn.hpp"

namespace Chicane
{
    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
        const std::vector<Actor*>& inIgnoredActors
    );
}