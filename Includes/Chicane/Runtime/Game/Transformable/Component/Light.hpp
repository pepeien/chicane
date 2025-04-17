#pragma once

#include "Core.hpp"
#include "Runtime/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    class CHICANE CLight : public CView
    {
    public:
        CLight();
        virtual ~CLight() = default;
    
    protected:
        void onTransform() override;
    }; 
}