#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    class APawn;

    class CHICANE_RUNTIME Controller
    {
    public:
        using PawnObservable   = Observable<APawn*>;
        using PawnSubscription = Subscription<APawn*>;

    public:
        Controller();
        virtual ~Controller() = default;

    protected:
        virtual void onActivation() { return; }
        virtual void onEvent(const Window::Event& inEvent) { return; }

    public:
        // Lifecycle
        void activate();

        template<class T = APawn>
        const T* getPawn() const
        {
            return dynamic_cast<T*>(m_pawn);
        }

        PawnSubscription watchAttachment(
            PawnSubscription::NextCallback inNext,
            PawnSubscription::ErrorCallback inError = nullptr,
            PawnSubscription::CompleteCallback inComplete = nullptr
        );

        bool isAttached() const;
        void attachTo(APawn* inPawn);
        void deattach();

        // Mouse Events
        void bindEvent(Mouse::MotionEventCallback inCallback);
        void bindEvent(
            Mouse::Button inButton,
            Input::Status inStatus,
            Mouse::ButtonEventCallback inCallback
        );

        // Keyboard Events
        void bindEvent(
            Keyboard::Button inButton,
            Input::Status inStatus,
            Keyboard::EventCallback inCallback
        );

        // Controller Events
        void bindEvent(Gamepad::MotionEventCallback inCallback);
        void bindEvent(
            Gamepad::Button inButton,
            Input::Status inStatus,
            Gamepad::ButtonEventCallback inCallback
        );

    private:
        // Mouse Events
        void onMouseMotionEvent(void* inEvent);
        void onMouseButtonEvent(void* inEvent);

        // Keyboard Events
        void onKeyboardButtonEvent(void* inEvent);

        // Controller Events
        void onGamepadMotionEvent(void* inEvent);
        void onGamepadButtonEvent(void* inEvent);

        void clearEvents();

    private:
        // Pawn
        APawn*                                   m_pawn;
        PawnObservable                           m_pawnObservable;

        // Mouse Events
        Input::Events<Mouse::MotionEvent>        m_mouseMotionEvents;
        Input::PressableEvents<Mouse::Button>    m_mouseButtonEvents;   

        // Keyboard Events
        Input::PressableEvents<Keyboard::Button> m_keyboardKeyEvents;

        // Gamepad Events
        Input::Events<Gamepad::MotionEvent>      m_gamepadMotionEvents;
        Input::PressableEvents<Gamepad::Button>  m_gamepadButtonEvents;
    };
}