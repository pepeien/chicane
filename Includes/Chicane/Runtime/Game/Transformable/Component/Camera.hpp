#pragma once

#include "Chicane.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    class CCamera : public CView
    {
    public:
        CCamera();
        virtual ~CCamera() = default;

    protected:
        void onTransform() override;
    }; 
}