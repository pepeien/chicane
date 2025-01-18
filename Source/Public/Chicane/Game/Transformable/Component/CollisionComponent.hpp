#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CollisionComponent : public Component
    {
    public:
        CollisionComponent();
        virtual ~CollisionComponent() = default;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;
    };
}