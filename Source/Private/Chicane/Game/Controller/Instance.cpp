#include "Chicane/Game/Controller/Instance.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn.hpp"

namespace Chicane
{
    Controller::Controller()
        : m_pawn(nullptr),
        m_pawnObservable(std::make_unique<Observable<Pawn*>>()),
        m_mouseMotionEvents({}),
        m_mouseButtonEvents({}),
        m_keyboardButtonEvents({}),
        m_controllerMotionEvents({}),
        m_controllerButtonEvents({})
    {}

    void Controller::activate()
    {
        Application::setController(this);

        onActivation();
    }

    Subscription<Pawn*>* Controller::watchAttachment(
        std::function<void (Pawn*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<Pawn*>* subscription = m_pawnObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_pawn);

        return subscription;
    }

    bool Controller::isAttached() const
    {
        return m_pawn != nullptr;
    }

    void Controller::attachTo(Pawn* inPawn)
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

    void Controller::bindMouseMotionEvent(std::function<void(const SDL_MouseMotionEvent&)> inEvent)
    {
        m_mouseMotionEvents.push_back(inEvent);
    }

    void Controller::bindMouseButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent)
    {
        if (m_mouseButtonEvents.find(inButtonCode) == m_mouseButtonEvents.end())
        {
            m_mouseButtonEvents[inButtonCode] = {};
        }

        m_mouseButtonEvents.at(inButtonCode).push_back(inEvent);
    }

    void Controller::bindKeyboardButtonEvent(SDL_Scancode inButtonCode, std::function<void(bool)> inEvent)
    {
        if (m_keyboardButtonEvents.find(inButtonCode) == m_keyboardButtonEvents.end())
        {
            m_keyboardButtonEvents[inButtonCode] = {};
        }

        m_keyboardButtonEvents.at(inButtonCode).push_back(inEvent);
    }

    void Controller::bindControllerMotionEvent(std::function<void(const SDL_GamepadAxisEvent&)> inEvent)
    {
        m_controllerMotionEvents.push_back(inEvent);
    }

    void Controller::bindControllerButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent)
    {
        if (m_controllerButtonEvents.find(inButtonCode) == m_controllerButtonEvents.end())
        {
            m_controllerButtonEvents[inButtonCode] = {};
        }

        m_controllerButtonEvents.at(inButtonCode).push_back(inEvent);
    }

    void Controller::onEvent(const SDL_Event& inEvent)
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

        // Controller
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            onControllerMotionEvent(inEvent.gaxis);

            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            onControllerButtonEvent(inEvent.gbutton);

            break;

        default:
            break;
        }
    }

    void Controller::onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent)
    {
        for (auto& function : m_mouseMotionEvents)
        {
            function(inEvent);
        }
    }

    void Controller::onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent)
    {
        std::uint8_t key = inEvent.button;

        if (m_mouseButtonEvents.find(key) == m_mouseButtonEvents.end())
        {
            return;
        }

        for (auto& function : m_mouseButtonEvents.at(key))
        {
            function(inEvent.down);
        }
    }

    void Controller::onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent)
    {
        SDL_Scancode key = inEvent.scancode;

        if (m_keyboardButtonEvents.find(key) == m_keyboardButtonEvents.end())
        {
            return;
        }

        for (auto& function : m_keyboardButtonEvents.at(key))
        {
            function(inEvent.down);
        }
    }

    void Controller::onControllerMotionEvent(const SDL_GamepadAxisEvent& inEvent)
    {
        for (auto& function : m_controllerMotionEvents)
        {
            function(inEvent);
        }
    }

    void Controller::onControllerButtonEvent(const SDL_GamepadButtonEvent& inEvent)
    {
        std::uint8_t key = inEvent.button;

        if (m_controllerButtonEvents.find(key) == m_controllerButtonEvents.end())
        {
            return;
        }

        for (auto& function : m_controllerButtonEvents.at(key))
        {
            function(inEvent.down);
        }
    }

    void Controller::clearEvents()
    {
        m_mouseMotionEvents.clear();
        m_mouseButtonEvents.clear();

        m_keyboardButtonEvents.clear();

        m_controllerButtonEvents.clear();
        m_controllerMotionEvents.clear();
    }
}