#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME CCamera : public CView
    {
    public:
        CH_CONSTRUCTOR()
        CCamera();

    protected:
        void onLoad() override;
        void onPropertyEdited(const String& inName) override;

    public:
        void onResize(const Vec<2, std::uint32_t>& inSize) override;

    public:
        CH_FIELD()
        String lookAt;
    };
}