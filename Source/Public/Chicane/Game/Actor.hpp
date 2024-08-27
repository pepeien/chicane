#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{
    class Actor : public Transformable
    {
    public:
        Actor();
        virtual ~Actor() = default;

    public:
        virtual void onTick(float inDeltaTime) { return; };

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);

    protected:
        bool m_canTick;
    };
} 