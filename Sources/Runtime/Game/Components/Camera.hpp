#pragma once

#include "Base.hpp"

#include "Game/Components/Actor.hpp"

namespace Chicane
{
    class CameraComponent : public ActorComponent
    {
    public:
        CameraComponent()
            : ActorComponent(),
            m_isUsingOwnerRotation(false)
        {}
    
    protected:
        bool m_isUsingOwnerRotation; 
    };
}