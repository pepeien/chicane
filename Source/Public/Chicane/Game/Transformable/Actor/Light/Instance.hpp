#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/LightComponent.hpp"

namespace Chicane
{
    class Light : public Actor
    {
    public:
        Light();
        virtual ~Light() = default;

    protected:
        LightComponent* m_light;
    };
}