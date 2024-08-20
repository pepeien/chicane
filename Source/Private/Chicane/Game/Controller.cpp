#include "Chicane/Game/Controller.hpp"

#include "Chicane/Game/Pawn.hpp"

namespace Chicane
{
    Controller::Controller()
        : m_pawn(nullptr),
        m_pawnObservable(std::make_unique<Observable<Pawn*>>())
    {}

    void Controller::observeAttachment(
        std::function<void (Pawn*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_pawnObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }

    bool Controller::isAttached()
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
        m_mouseMotionEvent = inEvent;
    }

    void Controller::bindMouseButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent)
    {
        m_mouseButtonEvents[inButtonCode] = inEvent;
    }

    void Controller::bindKeyboardButtonEvent(SDL_Scancode inButtonCode, std::function<void(bool)> inEvent)
    {
        m_keyboardButtonEvents[inButtonCode] = inEvent;
    }

    void Controller::bindControllerMotionEvent(std::function<void(const SDL_ControllerAxisEvent&)> inEvent)
    {
        m_controllerMotionEvent = inEvent;
    }

    void Controller::bindControllerButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent)
    {
        m_controllerButtonEvents[inButtonCode] = inEvent;
    }

    void Controller::onEvent(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        // Mouse
        case SDL_MOUSEMOTION:
            if (m_mouseMotionEvent)
            {
                m_mouseMotionEvent(inEvent.motion);
            }

            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            onMouseButtonEvent(inEvent.button);

            break;

        // Keyboard
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            onKeyboardButtonEvent(inEvent.key);

            break;

        // Controller
        case SDL_CONTROLLERAXISMOTION:
            if (m_controllerMotionEvent)
            {
                m_controllerMotionEvent(inEvent.caxis);
            }

            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            onControllerButtonEvent(inEvent.cbutton);

            break;

        default:
            break;
        }
    }

    void Controller::onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent)
    {
        std::uint8_t key = inEvent.button;

        if (m_mouseButtonEvents.find(key) == m_mouseButtonEvents.end())
        {
            return;
        }

        m_mouseButtonEvents.at(key)(inEvent.state == SDL_PRESSED);
    }

    void Controller::onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent)
    {
        SDL_Scancode key = inEvent.keysym.scancode;

        if (m_keyboardButtonEvents.find(key) == m_keyboardButtonEvents.end())
        {
            return;
        }

        m_keyboardButtonEvents.at(key)(inEvent.state == SDL_PRESSED);
    }

    void Controller::onControllerButtonEvent(const SDL_ControllerButtonEvent& inEvent)
    {
        std::uint8_t key = inEvent.button;

        if (m_controllerButtonEvents.find(key) == m_controllerButtonEvents.end())
        {
            return;
        }

        m_controllerButtonEvents.at(key)(inEvent.state == SDL_PRESSED);
    }

    void Controller::clearEvents()
    {
        m_mouseMotionEvent = nullptr;
        m_mouseButtonEvents.clear();

        m_keyboardButtonEvents.clear();

        m_controllerMotionEvent = nullptr;
        m_controllerButtonEvents.clear();
    }
}