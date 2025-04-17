#pragma once

#include "Core.hpp"
#include "Runtime/Game/Transformable/Actor/Instance.hpp"
#include "Runtime/Game/Transformable/Component/Light.hpp"

namespace Chicane
{
    class CHICANE ALight : public Actor
    {
    public:
        ALight();
        virtual ~ALight() = default;

    protected:
        CLight* m_light;
    };
}