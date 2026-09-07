#pragma once

#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character/Navigation/Type.hpp"

namespace Editor
{
    class Navigation
    {
    public:
        Navigation();

    public:
        void attach(Chicane::ACharacter* inCharacter, Chicane::CCamera* inCamera);
        void bind(Chicane::Controller* inController);
        void apply();

    protected:
        void onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent);
        void onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent);
        void onMouseWheel(const Chicane::Input::MouseWheelEvent& inEvent);
        void onMouseButton(Chicane::Input::MouseButton inButton, bool bInIsHeld);
        void onModifierKey(Chicane::Input::KeyboardButton inButton, bool bInIsHeld);

        void orbit(float inYaw, float inPitch);
        void pan(float inX, float inY);
        void zoom(float inDelta);
        void start();
        void stop();
        bool isViewportHovered() const;

    private:
        Chicane::ACharacter* m_character;
        Chicane::CCamera*    m_camera;
        float                m_yaw;
        float                m_pitch;
        float                m_distance;
        Chicane::Vec3        m_pivot;
        bool                 m_bShift;
        bool                 m_bCtrl;
        bool                 m_bAlt;
        bool                 m_bMiddle;
        bool                 m_bLeft;
        bool                 m_bRight;
        NavigationType       m_type;
    };
}
