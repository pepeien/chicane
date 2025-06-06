#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game.hpp"

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

    void onLeftClick();
    void onRightClick();

    // Keyboard
    void onMoveForward();
    void onMoveBackward();
    void onMoveLeft();
    void onMoveRight();

    // Actions
    void onShoot();
    void onLook(float inX, float inY);
    void onJump();

private:
    // Camera
    Chicane::CCamera* m_camera;

    // Mesh
    Chicane::CMesh*   m_wand;
    Chicane::CMesh*   m_body;

    // Audio
    Chicane::CAudio*  m_hitAudio;
    Chicane::CAudio*  m_victoryAudio;
};