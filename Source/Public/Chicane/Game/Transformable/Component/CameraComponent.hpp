#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/ViewComponent.hpp"

namespace Chicane
{
    class CameraComponent : public ViewComponent
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;
    
    protected:
        virtual void onTransform();
    }; 
}