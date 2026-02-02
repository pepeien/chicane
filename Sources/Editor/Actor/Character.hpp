#pragma once

#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

namespace Editor
{
    class Character : public Chicane::ACharacter
    {
    public:
        Character();

    protected:
        void onControlAttachment() override;

    private:
        // Gamepad
        void onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent);

        // Mouse
        void onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent);

        // Keyboard
        void onMoveForward();
        void onMoveBackward();
        void onMoveLeft();
        void onMoveRight();
        void onMoveUp();
        void onMoveDown();

        void onLook(float inX, float inY);

    private:
        Chicane::CCamera* m_camera;
    };
}