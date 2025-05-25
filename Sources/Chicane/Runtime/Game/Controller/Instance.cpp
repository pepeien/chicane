#include "Chicane/Runtime/Game/Controller.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Pawn.hpp"

namespace Chicane
{
    Controller::Controller()
        : m_pawn(nullptr),
        m_pawnObservable(std::make_unique<PawnObservable>()),
        m_mouseMotionEvents({}),
        m_mouseButtonEvents({}),
        m_keyboardKeyEvents({}),
        m_gamepadMotionEvents({}),
        m_gamepadButtonEvents({})
    {}

    void Controller::activate()
    {
        Application::setController(this);

        onActivation();
    }

    void Controller::handle(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        // Mouse
        case SDL_EVENT_MOUSE_MOTION:
            onMouseMotionEvent(inEvent.motion);

            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            onMouseButtonEvent(inEvent.button);

            break;

        // Keyboard
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            onKeyboardButtonEvent(inEvent.key);

            break;

        // Gamepad
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            onGamepadMotionEvent(inEvent.gaxis);

            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            onGamepadButtonEvent(inEvent.gbutton);

            break;

        default:
            break;
        }

        onEvent(inEvent);
    }

    Controller::PawnSubscription* Controller::watchAttachment(
        PawnSubscription::NextCallback inNext,
        PawnSubscription::ErrorCallback inError,
        PawnSubscription::CompleteCallback inComplete
    )
    {
        return m_pawnObservable
            ->subscribe(inNext, inError, inComplete)
            ->next(m_pawn);
    }

    bool Controller::isAttached() const
    {
        return m_pawn != nullptr;
    }

    void Controller::attachTo(APawn* inPawn)
    {
        if (isAttached())
        {
            m_pawnObservable->error("Pawn is null");

            return;
        }

        if (inPawn->isControlled())
        {
            m_pawnObservable->error("This pawn is currently possesed");

            return;
        }

        clearEvents();

        inPawn->attachController(this);

        m_pawn = inPawn;

        m_pawnObservable->next(inPawn);
    }

    void Controller::deattach()
    {
        if (!isAttached())
        {
            m_pawnObservable->error("The controller doesn't possess a Pawn");

            return;
        }

        clearEvents();

        m_pawn->deattachController();
        m_pawn = nullptr;

        m_pawnObservable->next(nullptr);
    }

    void Controller::bindEvent(Mouse::MotionEvent inEvent)
    {
        m_mouseMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(Mouse::Button inButton, Input::Status inStatus, Mouse::ButtonEvent inEvent)
    {
        m_mouseButtonEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(Keyboard::Button inButton, Input::Status inStatus, Keyboard::Event inEvent)
    {
        m_keyboardKeyEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(Gamepad::MotionEvent inEvent)
    {
        m_gamepadMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(Gamepad::Button inButton, Input::Status inStatus, Gamepad::ButtonEvent inEvent)
    {
        m_gamepadButtonEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::onMouseMotionEvent(const Mouse::MotionEventData& inData)
    {
        m_mouseMotionEvents.exec(inData);
    }

    void Controller::onMouseButtonEvent(const Mouse::ButtonEventData& inEvent)
    {
        Mouse::Button button = (Mouse::Button) inEvent.button;
        Input::Status status = inEvent.down ?
            Input::Status::Pressed :
            Input::Status::Released;

        m_mouseButtonEvents.exec(button, status);
    }

    void Controller::onKeyboardButtonEvent(const Keyboard::EventData& inEvent)
    {
        Keyboard::Button button = (Keyboard::Button) inEvent.scancode;
        Input::Status status = inEvent.down ?
            Input::Status::Pressed :
            Input::Status::Released;

        m_keyboardKeyEvents.exec(button, status);
    }

    void Controller::onGamepadMotionEvent(const Gamepad::MotionEventData& inEvent)
    {
        m_gamepadMotionEvents.exec(inEvent);
    }

    void Controller::onGamepadButtonEvent(const Gamepad::ButtonEventData& inEvent)
    {
        Gamepad::Button button = (Gamepad::Button) inEvent.button;
        Input::Status status = inEvent.down ?
            Input::Status::Pressed :
            Input::Status::Released;

        m_gamepadButtonEvents.exec(button, status);
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