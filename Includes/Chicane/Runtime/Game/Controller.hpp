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
        virtual void onEvent(const SDL_Event& inEvent) { return; }

    public:
        // Lifecycle
        void activate();
        void handle(const SDL_Event& inEvent);

        template<class T = APawn>
        const T* getPawn() const
        {
            return dynamic_cast<T*>(m_pawn);
        }

        PawnSubscription* watchAttachment(
            PawnSubscription::NextCallback inNext,
            PawnSubscription::ErrorCallback inError = nullptr,
            PawnSubscription::CompleteCallback inComplete = nullptr
        );

        bool isAttached() const;
        void attachTo(APawn* inPawn);
        void deattach();

        // Mouse Events
        void bindEvent(Mouse::MotionEvent inEvent);
        void bindEvent(Mouse::Button inButton, Input::Status inStatus, Mouse::ButtonEvent inEvent);

        // Keyboard Events
        void bindEvent(Keyboard::Button inButton, Input::Status inStatus, Keyboard::Event inEvent);

        // Controller Events
        void bindEvent(Gamepad::MotionEvent inEvent);
        void bindEvent(Gamepad::Button inButton, Input::Status inStatus, Gamepad::ButtonEvent inEvent);

    private:
        // Mouse Events
        void onMouseMotionEvent(const Mouse::MotionEventData& inData);
        void onMouseButtonEvent(const Mouse::ButtonEventData& inEvent);

        // Keyboard Events
        void onKeyboardButtonEvent(const Keyboard::EventData& inEvent);

        // Controller Events
        void onGamepadMotionEvent(const Gamepad::MotionEventData& inEvent);
        void onGamepadButtonEvent(const Gamepad::ButtonEventData& inEvent);

        void clearEvents();

    private:
        // Pawn
        APawn*                                   m_pawn;
        std::unique_ptr<PawnObservable>          m_pawnObservable;

        // Mouse Events
        Input::Events<Mouse::MotionEventData>    m_mouseMotionEvents;
        Input::PressableEvents<Mouse::Button>    m_mouseButtonEvents;   

        // Keyboard Events
        Input::PressableEvents<Keyboard::Button> m_keyboardKeyEvents;

        // Gamepad Events
        Input::Events<Gamepad::MotionEventData>  m_gamepadMotionEvents;
        Input::PressableEvents<Gamepad::Button>  m_gamepadButtonEvents;
    };
}