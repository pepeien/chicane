#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ALight : public Actor
    {
    public:
        ALight();
        virtual ~ALight() = default;

    protected:
        CLight* m_light;
    };
}