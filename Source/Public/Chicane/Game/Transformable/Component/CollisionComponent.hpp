#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component.hpp"

namespace Chicane
{
    class CollisionComponent : public Component
    {
    public:
        CollisionComponent();
        virtual ~CollisionComponent() = default;

    protected:
        void onActivation() override;
        void onDeactivation() override;
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;
    };
}