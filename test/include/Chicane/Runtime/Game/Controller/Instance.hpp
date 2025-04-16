#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game/Controller/Gamepad.hpp"
#include "Chicane/Runtime/Game/Controller/Keyboard.hpp"
#include "Chicane/Runtime/Game/Controller/Mouse.hpp"

namespace Chicane
{
    class APawn;

    namespace Controller
    {
        class CHICANE Instance
        {
        public:
            Instance();
            virtual ~Instance() = default;

        public:
            virtual void onActivation() { return; };

        public:
            // Lifecycle
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
            void bindEvent(Mouse::MotionEventFunction inEvent);
            void bindEvent(Mouse::Button inButton, EventStatus inStatus, Mouse::ButtonEventFunction inEvent);

            // Keyboard Events
            void bindEvent(Keyboard::Key inKey, EventStatus inStatus, Keyboard::KeyEventFunction inEvent);

            // Controller Events
            void bindEvent(Gamepad::MotionEventFunction inEvent);
            void bindEvent(Gamepad::Button inButton, EventStatus inStatus, Gamepad::ButtonEventFunction inEvent);

            void onEvent(const SDL_Event& inEvent);

        private:
            // Mouse Events
            void onMouseMotionEvent(const SDL_MouseMotionEvent& inData);
            void onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent);

            // Keyboard Events
            void onKeyboardKeyEvent(const SDL_KeyboardEvent& inEvent);

            // Controller Events
            void onGamepadMotionEvent(const SDL_GamepadAxisEvent& inEvent);
            void onGamepadButtonEvent(const SDL_GamepadButtonEvent& inEvent);

            void repeatEvents();
            void clearEvents();

        private:
            // Pawn
            APawn*                              m_pawn;
            std::unique_ptr<Observable<APawn*>> m_pawnObservable;

            // Mouse Events
            Events<Mouse::MotionEvent>          m_mouseMotionEvents;
            PressableEvents<Mouse::Button>      m_mouseButtonEvents;   

            // Keyboard Events
            PressableEvents<Keyboard::Key>      m_keyboardKeyEvents;

            // Gamepad Events
            Events<Gamepad::MotionEvent>        m_gamepadMotionEvents;
            PressableEvents<Gamepad::Button>    m_gamepadButtonEvents;
        };
    }
}