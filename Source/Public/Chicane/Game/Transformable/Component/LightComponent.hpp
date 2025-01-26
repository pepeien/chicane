#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/ViewComponent.hpp"

namespace Chicane
{
    class LightComponent : public ViewComponent
    {
    public:
        LightComponent();
        virtual ~LightComponent() = default;
    
    protected:
        void onTransform() override;
    }; 
}