#pragma once

#include <Chicane/Core/Input/Keyboard/Button/Modifier.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character/Navigation/Type.hpp"

namespace Editor
{
    class Navigation
    {
    public:
        static constexpr inline const float ORBIT_SPEED     = 0.4f;
        static constexpr inline const float PAN_SPEED       = 0.0025f;
        static constexpr inline const float ZOOM_DRAG_SPEED = 0.01f;
        static constexpr inline const float ZOOM_WHEEL      = 0.85f;
        static constexpr inline const float MIN_DISTANCE    = 0.25f;
        static constexpr inline const float MAX_DISTANCE    = 100000.0f;
        static constexpr inline const float MIN_PITCH       = -89.0f;
        static constexpr inline const float MAX_PITCH       = 89.0f;

    public:
        Navigation();

    public:
        void attach(Chicane::ACharacter* inCharacter, Chicane::CCamera* inCamera);
        void bind(Chicane::Controller* inController);
        void frame(const Chicane::Vec3& inPosition, const Chicane::Vec3& inPivot);
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
        bool has(Chicane::Input::KeyboardButtonModifier inModifier) const;
        bool has(Chicane::Input::MouseButton inButton) const;

    private:
        Chicane::ACharacter*                   m_character;
        Chicane::CCamera*                      m_camera;
        float                                  m_yaw;
        float                                  m_pitch;
        float                                  m_distance;
        Chicane::Vec3                          m_pivot;
        Chicane::Input::KeyboardButtonModifier m_modifiers;
        std::uint8_t                           m_buttons;
        NavigationType                         m_type;
    };
}
