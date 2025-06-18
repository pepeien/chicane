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
        void bindEvent(Input::MouseMotionEventCallback inCallback);
        void bindEvent(
            Input::MouseButton inButton,
            Input::Status inStatus,
            Input::MouseButtonEventCallback inCallback
        );

        // Keyboard Events
        void bindEvent(
            Input::KeyboardButton inButton,
            Input::Status inStatus,
            Input::KeyboardEventCallback inCallback
        );

        // Controller Events
        void bindEvent(Input::GamepadMotionEventCallback inCallback);
        void bindEvent(
            Input::GamepadButton inButton,
            Input::Status inStatus,
            Input::GamepadButtonEventCallback inCallback
        );

        // Device
        bool isConnectedTo(Input::DeviceType inType, Input::DeviceID inId) const;
        bool isConnectedTo(Input::DeviceType inType) const;
        void connectTo(Input::DeviceType inType, Input::DeviceID inId);
        void disconnectFrom(Input::DeviceType inType);

    private:
        // Mouse Events
        void onMouseMotionEvent(void* inEvent);
        void onMouseButtonEvent(void* inEvent);

        // Keyboard Events
        void onKeyboardButtonEvent(void* inEvent);

        // Controller Events
        void onGamepadMotionEvent(void* inEvent);
        void onGamepadButtonEvent(void* inEvent);

        // Events
        void setupEvents();
        void clearEvents();

        // Device
        void setupDevices();
        void setupDefaultGamepad();
        void setupDefaultKeyboard();
        void setupDefaultMouse();

    private:
        // Pawn
        APawn*                                        m_pawn;
        PawnObservable                                m_pawnObservable;

        // Mouse Events
        Input::Events<Input::MouseMotionEvent>        m_mouseMotionEvents;
        Input::PressableEvents<Input::MouseButton>    m_mouseButtonEvents;   

        // Keyboard Events
        Input::PressableEvents<Input::KeyboardButton> m_keyboardKeyEvents;

        // Gamepad Events
        Input::Events<Input::GamepadMotionEvent>      m_gamepadMotionEvents;
        Input::PressableEvents<Input::GamepadButton>  m_gamepadButtonEvents;

        // Device
        Input::DeviceMap                              m_devices;
    };
}