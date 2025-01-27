#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/View.hpp"

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