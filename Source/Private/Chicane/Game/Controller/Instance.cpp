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
            m_keyboardButtonEvents({}),
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

        void Instance::bindMouseMotionEvent(MouseMotionEventFunction inEvent)
        {
            m_mouseMotionEvents.push_back(inEvent);
        }

        void Instance::bindMouseButtonEvent(MouseButton inButton, EventStatus inStatus, MouseButtonEventFunction inEvent)
        {
            if (m_mouseButtonEvents.find(inButton) == m_mouseButtonEvents.end())
            {
                m_mouseButtonEvents[inButton] = {};
            }

            MouseButtonEventMap& eventMap = m_mouseButtonEvents.at(inButton);

            if (eventMap.find(inStatus) == eventMap.end())
            {
                eventMap[inStatus] = {};
            }

            eventMap.at(inStatus).push_back(inEvent);
        }

        void Instance::bindKeyboardEvent(KeyboardKey inKey, EventStatus inStatus, KeyboardEventFunction inEvent)
        {
            if (m_keyboardButtonEvents.find(inKey) == m_keyboardButtonEvents.end())
            {
                m_keyboardButtonEvents[inKey] = {};
            }

            KeyboardEventMap& eventMap = m_keyboardButtonEvents.at(inKey);

            if (eventMap.find(inStatus) == eventMap.end())
            {
                eventMap[inStatus] = {};
            }

            eventMap.at(inStatus).push_back(inEvent);
        }

        void Instance::bindGamepadMotionEvent(GamepadMotionEventFunction inEvent)
        {
            m_gamepadMotionEvents.push_back(inEvent);
        }

        void Instance::bindGamepadButtonEvent(GamepadButton inButton, EventStatus inStatus, GamepadButtonEventFunction inEvent)
        {
            if (m_gamepadButtonEvents.find(inButton) == m_gamepadButtonEvents.end())
            {
                m_gamepadButtonEvents[inButton] = {};
            }

            GamepadButtonEventMap& eventMap = m_gamepadButtonEvents.at(inButton);

            if (eventMap.find(inStatus) == eventMap.end())
            {
                eventMap[inStatus] = {};
            }

            eventMap.at(inStatus).push_back(inEvent);
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

        void Instance::onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent)
        {
            for (auto& function : m_mouseMotionEvents)
            {
                function(inEvent);
            }
        }

        void Instance::onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent)
        {
            MouseButton button = (MouseButton) inEvent.button;

            if (m_mouseButtonEvents.find(button) == m_mouseButtonEvents.end())
            {
                return;
            }

            MouseButtonEventMap& eventMap = m_mouseButtonEvents.at(button);

            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            if (eventMap.find(status) == eventMap.end())
            {
                return;
            }

            for (auto& function : eventMap.at(status))
            {
                function();
            }
        }

        void Instance::onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent)
        {
            KeyboardKey key = (KeyboardKey) inEvent.scancode;

            if (m_keyboardButtonEvents.find(key) == m_keyboardButtonEvents.end())
            {
                return;
            }

            KeyboardEventMap& eventMap = m_keyboardButtonEvents.at(key);

            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            if (eventMap.find(status) == eventMap.end())
            {
                return;
            }

            for (auto& function : eventMap.at(status))
            {
                function();
            }
        }

        void Instance::onControllerMotionEvent(const SDL_GamepadAxisEvent& inEvent)
        {
            for (auto& function : m_gamepadMotionEvents)
            {
                function(inEvent);
            }
        }

        void Instance::onControllerButtonEvent(const SDL_GamepadButtonEvent& inEvent)
        {
            GamepadButton button = (GamepadButton) inEvent.button;

            if (m_gamepadButtonEvents.find(button) == m_gamepadButtonEvents.end())
            {
                return;
            }

            GamepadButtonEventMap& eventMap = m_gamepadButtonEvents.at(button);

            EventStatus status = inEvent.down ? EventStatus::Pressed : EventStatus::Released;

            if (eventMap.find(status) == eventMap.end())
            {
                return;
            }

            for (auto& function : eventMap.at(status))
            {
                function();
            }
        }

        void Instance::clearEvents()
        {
            m_mouseMotionEvents.clear();
            m_mouseButtonEvents.clear();

            m_keyboardButtonEvents.clear();

            m_gamepadButtonEvents.clear();
            m_gamepadMotionEvents.clear();
        }
    }
}