#include "Chicane/Runtime/Game/Controller.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Pawn.hpp"

namespace Chicane
{
    Controller::Controller()
        : m_pawn(nullptr),
        m_pawnObservable({}),
        m_mouseMotionEvents({}),
        m_mouseButtonEvents({}),
        m_keyboardKeyEvents({}),
        m_gamepadMotionEvents({}),
        m_gamepadButtonEvents({})
    {
        Application::getWindow()->watchEvent(
            [this](Window::Event inEvent)
            {
                m_mouseButtonEvents.repeat();
                m_keyboardKeyEvents.repeat();
                m_gamepadButtonEvents.repeat();

                switch (inEvent.type)
                {
                // Mouse
                case Window::EventType::MouseMotion:
                    onMouseMotionEvent(inEvent.data);

                    break;

                case Window::EventType::MouseButtonDown:
                case Window::EventType::MouseButtonUp:
                    onMouseButtonEvent(inEvent.data);

                    break;

                // Keyboard
                case Window::EventType::KeyDown:
                case Window::EventType::KeyUp:
                    onKeyboardButtonEvent(inEvent.data);

                    break;

                // Gamepad
                case Window::EventType::GamepadAxisMotion:
                    onGamepadMotionEvent(inEvent.data);

                    break;

                case Window::EventType::GamepadButtonDown:
                case Window::EventType::GamepadButtonUp:
                    onGamepadButtonEvent(inEvent.data);

                    break;

                default:
                    break;
                }

                onEvent(inEvent);
            }
        );
    }

    void Controller::activate()
    {
        Application::setController(this);

        onActivation();
    }

    Controller::PawnSubscription Controller::watchAttachment(
        PawnSubscription::NextCallback inNext,
        PawnSubscription::ErrorCallback inError,
        PawnSubscription::CompleteCallback inComplete
    )
    {
        return m_pawnObservable
            .subscribe(inNext, inError, inComplete)
            .next(m_pawn);
    }

    bool Controller::isAttached() const
    {
        return m_pawn != nullptr;
    }

    void Controller::attachTo(APawn* inPawn)
    {
        if (isAttached())
        {
            m_pawnObservable.error("Pawn is null");

            return;
        }

        if (inPawn->isControlled())
        {
            m_pawnObservable.error("This pawn is currently possesed");

            return;
        }

        clearEvents();

        inPawn->attachController(this);

        m_pawn = inPawn;

        m_pawnObservable.next(inPawn);
    }

    void Controller::deattach()
    {
        if (!isAttached())
        {
            m_pawnObservable.error("The controller doesn't possess a Pawn");

            return;
        }

        clearEvents();

        m_pawn->deattachController();
        m_pawn = nullptr;

        m_pawnObservable.next(nullptr);
    }

    void Controller::bindEvent(Mouse::MotionEventCallback inEvent)
    {
        m_mouseMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(
        Mouse::Button inButton,
        Input::Status inStatus,
        Mouse::ButtonEventCallback inEvent
    )
    {
        m_mouseButtonEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(
        Keyboard::Button inButton,
        Input::Status inStatus,
        Keyboard::EventCallback inEvent
    )
    {
        m_keyboardKeyEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(Gamepad::MotionEventCallback inEvent)
    {
        m_gamepadMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(
        Gamepad::Button inButton,
        Input::Status inStatus,
        Gamepad::ButtonEventCallback inEvent
    )
    {
        m_gamepadButtonEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::onMouseMotionEvent(void* inEvent)
    {
        Mouse::MotionEvent event = *static_cast<Mouse::MotionEvent*>(inEvent);

        m_mouseMotionEvents.exec(event);
    }

    void Controller::onMouseButtonEvent(void* inEvent)
    {
        Mouse::ButtonEvent event = *static_cast<Mouse::ButtonEvent*>(inEvent);

        m_mouseButtonEvents.exec(event.button, event.status);
    }

    void Controller::onKeyboardButtonEvent(void* inEvent)
    {
        Keyboard::Event event = *static_cast<Keyboard::Event*>(inEvent);

        m_keyboardKeyEvents.exec(event.button, event.status);
    }

    void Controller::onGamepadMotionEvent(void* inEvent)
    {
        Gamepad::MotionEvent event = *static_cast<Gamepad::MotionEvent*>(inEvent);

        m_gamepadMotionEvents.exec(event);
    }

    void Controller::onGamepadButtonEvent(void* inEvent)
    {
        Gamepad::ButtonEvent event = *static_cast<Gamepad::ButtonEvent*>(inEvent);

        m_gamepadButtonEvents.exec(event.button, event.status);
    }

    void Controller::clearEvents()
    {
        m_mouseMotionEvents.clear();
        m_mouseButtonEvents.clear();

        m_keyboardKeyEvents.clear();

        m_gamepadButtonEvents.clear();
        m_gamepadMotionEvents.clear();
    }
}