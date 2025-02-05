#include "Chicane/Game/Controller/Instance.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn.hpp"

namespace Chicane
{
    namespace Controller
    {
        Instance::Instance()
            : m_pawn(nullptr),
            m_pawnObservable(std::make_unique<Observable<APawn*>>()),
            m_mouseMotionEvents({}),
            m_mouseButtonEvents({}),
            m_keyboardKeyEvents({}),
            m_gamepadMotionEvents({}),
            m_gamepadButtonEvents({})
        {}

        void Instance::activate()
        {
            Application::setController(this);

            onActivation();
        }

        Subscription<APawn*>* Instance::watchAttachment(
            std::function<void (APawn*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        )
        {
            Subscription<APawn*>* subscription = m_pawnObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_pawn);

            return subscription;
        }

        bool Instance::isAttached() const
        {
            return m_pawn != nullptr;
        }

        void Instance::attachTo(APawn* inPawn)
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

        void Instance::deattach()
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

        void Instance::bindEvent(MouseMotionEventFunction inEvent)
        {
            m_mouseMotionEvents.bind(inEvent);
        }

        void Instance::bindEvent(MouseButton inButton, EventStatus inStatus, MouseButtonEventFunction inEvent)
        {
            m_mouseButtonEvents.bind(inButton, inStatus, inEvent);
        }

        void Instance::bindEvent(KeyboardKey inKey, EventStatus inStatus, KeyboardEventFunction inEvent)
        {
            m_keyboardKeyEvents.bind(inKey, inStatus, inEvent);
        }

        void Instance::bindEvent(GamepadMotionEventFunction inEvent)
        {
            m_gamepadMotionEvents.bind(inEvent);
        }

        void Instance::bindEvent(GamepadButton inButton, EventStatus inStatus, GamepadButtonEventFunction inEvent)
        {
            m_gamepadButtonEvents.bind(inButton, inStatus, inEvent);
        }

        void Instance::onEvent(const SDL_Event& inEvent)
        {
            if (inEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (inEvent.button.button == SDL_BUTTON_LEFT)
                {
                    Chicane::Application::getWindow()->focus();
                }
            }

            if (!Application::getWindow()->isFocused())
            {
                return;
            }

            repeatEvents();

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
                onKeyboardKeyEvent(inEvent.key);

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
        }

        void Instance::onMouseMotionEvent(const SDL_MouseMotionEvent& inData)
        {
            m_mouseMotionEvents.exec(inData);
        }

        void Instance::onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent)
        {
            MouseButton button = (MouseButton) inEvent.button;
            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            m_mouseButtonEvents.exec(button, status);
        }

        void Instance::onKeyboardKeyEvent(const SDL_KeyboardEvent& inEvent)
        {
            KeyboardKey key = (KeyboardKey) inEvent.scancode;
            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            m_keyboardKeyEvents.exec(key, status);
        }

        void Instance::onGamepadMotionEvent(const SDL_GamepadAxisEvent& inEvent)
        {
            m_gamepadMotionEvents.exec(inEvent);
        }

        void Instance::onGamepadButtonEvent(const SDL_GamepadButtonEvent& inEvent)
        {
            GamepadButton button = (GamepadButton) inEvent.button;
            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            m_gamepadButtonEvents.exec(button, status);
        }

        void Instance::repeatEvents()
        {
            m_mouseButtonEvents.repeat();
            m_keyboardKeyEvents.repeat();
            m_gamepadButtonEvents.repeat();
        }

        void Instance::clearEvents()
        {
            m_mouseMotionEvents.clear();
            m_mouseButtonEvents.clear();

            m_keyboardKeyEvents.clear();

            m_gamepadButtonEvents.clear();
            m_gamepadMotionEvents.clear();
        }
    }
}