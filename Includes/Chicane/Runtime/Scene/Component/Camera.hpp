#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Component | Camera")
    class CHICANE_RUNTIME CCamera : public CView
    {
    public:
        CH_CONSTRUCTOR()
        CCamera();

    public:
        void onResize(const Vec<2, std::uint32_t>& inSize) override;

    public:
        Transform2D projectToScreenSpace(
            const Object* inTarget, const Vec2& inViewport = Vec2::sZero(), const Vec3& inOffset = Vec3::sZero()
        ) const;
    };
}
