#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Controller/Gamepad.hpp"
#include "Chicane/Game/Controller/Keyboard.hpp"
#include "Chicane/Game/Controller/Mouse.hpp"

namespace Chicane
{
    class APawn;

    namespace Controller
    {
        class Instance
        {
        public:
            Instance();
            virtual ~Instance() = default;

        public:
            virtual void onActivation() { return; };

        public:
            void activate();

            template<class T = APawn>
            const T* getPawn() const
            {
                return dynamic_cast<T*>(m_pawn);
            }

            Subscription<APawn*>* watchAttachment(
                std::function<void (APawn*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

            bool isAttached() const;
            void attachTo(APawn* inPawn);
            void deattach();

            // Mouse Events
            void bindMouseMotionEvent(MouseMotionEventFunction inEvent);
            void bindMouseButtonEvent(MouseButton inButton, EventStatus inStatus, MouseButtonEventFunction inEvent);

            // Keyboard Events
            void bindKeyboardEvent(KeyboardKey inKey, EventStatus inStatus, KeyboardEventFunction inEvent);

            // Controller Events
            void bindGamepadMotionEvent(GamepadMotionEventFunction inEvent);
            void bindGamepadButtonEvent(GamepadButton inButton, EventStatus inStatus, GamepadButtonEventFunction inEvent);

            void onEvent(const SDL_Event& inEvent);

        private:
            // Events
            void onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent);
            void onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent);
            void onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent);
            void onControllerMotionEvent(const SDL_GamepadAxisEvent& inEvent);
            void onControllerButtonEvent(const SDL_GamepadButtonEvent& inEvent);

            void clearEvents();

        private:
            // Pawn
            APawn*                              m_pawn;
            std::unique_ptr<Observable<APawn*>> m_pawnObservable;

            // Mouse Events
            MouseMotionEvents                   m_mouseMotionEvents;
            MouseButtonEvents                   m_mouseButtonEvents;

            // Keyboard Events
            KeyboardEvents                      m_keyboardButtonEvents;

            // Controller Events
            GamepadMotionEvents                 m_gamepadMotionEvents;
            GamepadButtonEvents                 m_gamepadButtonEvents;
        };
    }
}