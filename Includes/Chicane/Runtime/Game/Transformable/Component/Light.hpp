#pragma once

#include "Chicane.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    class CLight : public CView
    {
    public:
        CLight();
        virtual ~CLight() = default;
    
    protected:
        void onTransform() override;
    }; 
}