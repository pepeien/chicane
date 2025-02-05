#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/View.hpp"

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