#pragma once

#include <cstdint>

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

    // Keyboard
    void onMoveKey(Chicane::Input::KeyboardButton inButton, bool inHeld);

    // Actions
    void onShoot();
    void onLook(float inX, float inY);
    void onJump();
    void refreshMoveInput();

private:
    std::uint8_t m_moveBits;
    float   m_padForward;
    float   m_padRight;

    // View
    Chicane::CCamera* m_camera;

    // Mesh
    Chicane::CMesh* m_wand;
    Chicane::CMesh* m_body;

    // Sound
    Chicane::CSound* m_victorySound;
};