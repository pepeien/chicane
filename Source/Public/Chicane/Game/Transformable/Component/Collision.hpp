#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CCollision : public Component
    {
    public:
        CCollision();
        virtual ~CCollision() = default;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;
    };
}