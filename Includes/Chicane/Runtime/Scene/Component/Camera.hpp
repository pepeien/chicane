#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CCamera : public CView
    {
    public:
        CCamera();

    public:
        void onResize(const Vec<2, std::uint32_t>& inSize) override;
    };
}