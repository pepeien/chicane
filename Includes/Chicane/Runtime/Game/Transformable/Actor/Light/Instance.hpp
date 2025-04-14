#pragma once

#include "Chicane.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Light.hpp"

namespace Chicane
{
    class ALight : public Actor
    {
    public:
        ALight();
        virtual ~ALight() = default;

    protected:
        CLight* m_light;
    };
}