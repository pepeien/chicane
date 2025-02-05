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
            void bindEvent(MouseMotionEventFunction inEvent);
            void bindEvent(MouseButton inButton, EventStatus inStatus, MouseButtonEventFunction inEvent);

            // Keyboard Events
            void bindEvent(KeyboardKey inKey, EventStatus inStatus, KeyboardEventFunction inEvent);

            // Controller Events
            void bindEvent(GamepadMotionEventFunction inEvent);
            void bindEvent(GamepadButton inButton, EventStatus inStatus, GamepadButtonEventFunction inEvent);

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
            Events<MouseMotionEvent>            m_mouseMotionEvents;
            PressableEvents<MouseButton>        m_mouseButtonEvents;   

            // Keyboard Events
            PressableEvents<KeyboardKey>        m_keyboardKeyEvents;

            // Gamepad Events
            Events<GamepadMotionEvent>          m_gamepadMotionEvents;
            PressableEvents<GamepadButton>      m_gamepadButtonEvents;
        };
    }
}