#pragma once

#include "Base.hpp"

namespace Chicane
{
    class AEditorCharacter : public ACharacter
    {
    public:
        AEditorCharacter();

    protected:
        void onControlAttachment() override;

    private:
        void onLook(const Controller::Mouse::MotionEvent& inEvent);

        void onLeftClick();
        void onRightClick();

        void onMoveForward();
        void onMoveBackward();
        void onMoveLeft();
        void onMoveRight();
        void onMoveUp();
        void onMoveDown();

    private:
        CCamera* m_cameraComponent;
    };
}