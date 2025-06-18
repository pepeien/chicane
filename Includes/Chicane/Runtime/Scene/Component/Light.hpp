#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CLight : public CView
    {
    public:
        CLight();
        virtual ~CLight() = default;
    
    protected:
        void onTransform() override;
    }; 
}