#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CCamera : public CView
    {
    public:
        CCamera();
        virtual ~CCamera() = default;

    protected:
        void onTransform() override;
    }; 
}