#pragma once

#include "Core.hpp"

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
            void bindEvent(Input::Mouse::MotionEventFunction inEvent);
            void bindEvent(
                Input::Mouse::Button inButton,
                Input::Event::Status inStatus,
                Input::Mouse::ButtonEventFunction inEvent
            );

            // Keyboard Events
            void bindEvent(
                Input::Keyboard::Key inKey,
                Input::Event::Status inStatus,
                Input::Keyboard::KeyEventFunction inEvent
            );

            // Controller Events
            void bindEvent(Input::Gamepad::MotionEventFunction inEvent);
            void bindEvent(
                Input::Gamepad::Button inButton,
                Input::Event::Status inStatus,
                Input::Gamepad::ButtonEventFunction inEvent
            );

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
            APawn*                                            m_pawn;
            std::unique_ptr<Observable<APawn*>>               m_pawnObservable;

            // Mouse Events
            Input::Events<Input::Mouse::MotionEvent>          m_mouseMotionEvents;
            Input::PressableEvents<Input::Mouse::Button>      m_mouseButtonEvents;   

            // Keyboard Events
            Input::PressableEvents<Input::Keyboard::Key>      m_keyboardKeyEvents;

            // Gamepad Events
            Input::Events<Input::Gamepad::MotionEvent>        m_gamepadMotionEvents;
            Input::PressableEvents<Input::Gamepad::Button>    m_gamepadButtonEvents;
        };
    }
}