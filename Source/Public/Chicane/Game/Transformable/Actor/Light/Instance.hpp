#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Light.hpp"

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