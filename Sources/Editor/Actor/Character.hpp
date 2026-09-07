#pragma once

#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character/Navigation.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class Character : public Chicane::ACharacter
    {
    public:
        Character();

    protected:
        void onLoad() override;
        void onControlAttachment() override;

    private:
        Navigation        m_navigation;
        Chicane::CCamera* m_camera;
    };
}
