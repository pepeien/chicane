#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor/Component.hpp"

namespace Chicane
{
    class CollisionComponent : public ActorComponent
    {
    public:
        CollisionComponent();
        virtual ~CollisionComponent() = default;

    public:
        void onActivation() override;
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;
    };
}