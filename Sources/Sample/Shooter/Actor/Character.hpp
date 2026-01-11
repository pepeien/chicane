#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"

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
    Chicane::CMesh* m_wand;
    Chicane::CMesh* m_body;

    // Sound
    Chicane::CSound* m_hitSound;
    Chicane::CSound* m_victorySound;
};