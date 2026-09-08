#pragma once

#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/Sound.hpp>

class Character : public Chicane::ACharacter
{
public:
    Character();

protected:
    void onLoad() override;
    void onControlAttachment() override;
    void onInput() override;

private:
    // Gamepad
    void onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent);

    // Mouse
    void onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent);

    void onLeftClick();
    void onRightClick();

    void onMoveKey(Chicane::Input::KeyboardButton inButton, bool inHeld);

    // Actions
    void onShoot();
    void onLook(float inX, float inY);
    void onJump();
    void refreshMoveInput();

private:
    bool  m_bIsMovingForward;
    bool  m_bIsMovingBackward;
    bool  m_bIsMovingLeft;
    bool  m_bIsMovingRight;
    float m_padForward;
    float m_padRight;

    Chicane::CCamera* m_camera;
    Chicane::CMesh*   m_wand;
    Chicane::CMesh*   m_body;
    Chicane::CSound*  m_victorySound;
};
