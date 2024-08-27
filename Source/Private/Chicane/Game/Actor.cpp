#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_canTick(false)
    {}

    bool Actor::canTick() const
    {
        return m_canTick;
    }

    void Actor::setCanTick(bool inCanTick)
    {
        m_canTick = inCanTick;
    }
}