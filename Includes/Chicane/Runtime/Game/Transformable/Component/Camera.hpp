#pragma once

#include "Core.hpp"
#include "Runtime/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    class CHICANE CCamera : public CView
    {
    public:
        CCamera();
        virtual ~CCamera() = default;

    protected:
        void onTransform() override;
    }; 
}