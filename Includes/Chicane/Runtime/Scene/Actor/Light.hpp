#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Actor | Light")
    class CHICANE_RUNTIME ALight : public Actor
    {
    public:
        CH_CONSTRUCTOR()
        ALight();

    protected:
        void onLoad() override;

    public:
        CH_FIELD(Group = "Light")
        CLight* light;
    };
}
