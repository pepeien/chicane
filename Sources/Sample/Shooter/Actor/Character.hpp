#pragma once

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Sound.hpp>

CH_TYPE(Type = (Manual), Alias = (Character))
class Character : public Chicane::ACharacter
{
public:
    static constexpr inline const char* CAMERA_ID        = "First Person";
    static constexpr inline const char* VICTORY_SOUND_ID = "Victory";

public:
    CH_CONSTRUCTOR()
    Character();
    ~Character() override;

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

    Chicane::CCamera* camera();

private:
    bool  m_bIsMovingForward;
    bool  m_bIsMovingBackward;
    bool  m_bIsMovingLeft;
    bool  m_bIsMovingRight;
    float m_padForward;
    float m_padRight;

    Chicane::CCamera* m_camera;
    Chicane::CSound*  m_victorySound;

    Chicane::EventSubscription<std::uint32_t> m_scoreWatch;
};
