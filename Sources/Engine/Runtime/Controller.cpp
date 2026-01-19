#include "Chicane/Runtime/Controller.hpp"

#include <SDL3/SDL.h>

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

namespace Chicane
{
    static const std::vector<String> DEFAULT_KEYBOARDS = {
        "Keyboard", "HID Keyboard Device (0x046d/0xc232)"
    };

    Controller::Controller()
        : m_pawn(nullptr),
          m_pawnObservable({}),
          m_mouseMotionEvents({}),
          m_mouseButtonEvents({}),
          m_keyboardKeyEvents({}),
          m_gamepadMotionEvents({}),
          m_gamepadButtonEvents({}),
          m_devices({})
    {
        setupEvents();
        setupDevices();
    }

    void Controller::activate()
    {
        Application::getInstance().setController(this);

        onActivation();
    }

    Controller::PawnSubscription Controller::watchAttachment(
        PawnSubscription::NextCallback     inNext,
        PawnSubscription::ErrorCallback    inError,
        PawnSubscription::CompleteCallback inComplete
    )
    {
        return m_pawnObservable.subscribe(inNext, inError, inComplete).next(m_pawn);
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

    void Controller::bindEvent(Input::MouseMotionEventCallback inEvent)
    {
        m_mouseMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(
        Input::MouseButton inButton, Input::Status inStatus, Input::MouseButtonEventCallback inEvent
    )
    {
        m_mouseButtonEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(
        Input::KeyboardButton inButton, Input::Status inStatus, Input::KeyboardEventCallback inEvent
    )
    {
        m_keyboardKeyEvents.bind(inButton, inStatus, inEvent);
    }

    void Controller::bindEvent(Input::GamepadMotionEventCallback inEvent)
    {
        m_gamepadMotionEvents.bind(inEvent);
    }

    void Controller::bindEvent(
        Input::GamepadButton              inButton,
        Input::Status                     inStatus,
        Input::GamepadButtonEventCallback inEvent
    )
    {
        m_gamepadButtonEvents.bind(inButton, inStatus, inEvent);
    }

    bool Controller::isConnectedTo(Input::DeviceType inType, Input::DeviceID inId) const
    {
        if (!isConnectedTo(inType))
        {
            return true;
        }

        return m_devices.at(inType) == inId;
    }

    bool Controller::isConnectedTo(Input::DeviceType inType) const
    {
        return m_devices.find(inType) != m_devices.end();
    }

    void Controller::connectTo(Input::DeviceType inType, Input::DeviceID inId)
    {
        if (isConnectedTo(inType, inId))
        {
            return;
        }

        if (inType == Input::DeviceType::Gamepad)
        {
            if (!SDL_IsGamepad(inId))
            {
                return;
            }

            if (!SDL_OpenGamepad(inId))
            {
                throw std::runtime_error(String::sprint("Failed to open the [%d] gamepad", inId));
            }
        }

        m_devices[inType] = inId;
    }

    void Controller::disconnectFrom(Input::DeviceType inType)
    {
        if (!isConnectedTo(inType))
        {
            return;
        }

        m_devices.erase(inType);
    }

    void Controller::onMouseMotionEvent(void* inEvent)
    {
        Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent);

        if (!isConnectedTo(Input::DeviceType::Mouse, event.device))
        {
            return;
        }

        m_mouseMotionEvents.exec(event);
    }

    void Controller::onMouseButtonEvent(void* inEvent)
    {
        Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent);

        if (!isConnectedTo(Input::DeviceType::Mouse, event.device))
        {
            return;
        }

        m_mouseButtonEvents.exec(event.button, event.status);
    }

    void Controller::onKeyboardButtonEvent(void* inEvent)
    {
        Input::KeyboardEvent event = *static_cast<Input::KeyboardEvent*>(inEvent);

        if (!isConnectedTo(Input::DeviceType::Keyboard, event.device))
        {
            return;
        }

        m_keyboardKeyEvents.exec(event.button, event.status);
    }

    void Controller::onGamepadMotionEvent(void* inEvent)
    {
        Input::GamepadMotionEvent event = *static_cast<Input::GamepadMotionEvent*>(inEvent);

        if (!isConnectedTo(Input::DeviceType::Gamepad, event.device))
        {
            return;
        }

        m_gamepadMotionEvents.exec(event);
    }

    void Controller::onGamepadButtonEvent(void* inEvent)
    {
        Input::GamepadButtonEvent event = *static_cast<Input::GamepadButtonEvent*>(inEvent);

        if (!isConnectedTo(Input::DeviceType::Gamepad, event.device))
        {
            return;
        }

        m_gamepadButtonEvents.exec(event.button, event.status);
    }

    void Controller::setupEvents()
    {
        Application::getInstance().getWindow()->watchEvent([this](WindowEvent inEvent) {
            m_mouseButtonEvents.repeat();
            m_keyboardKeyEvents.repeat();
            m_gamepadButtonEvents.repeat();

            switch (inEvent.type)
            {
            // Gamepad
            case WindowEventType::GamepadAdded:
            case WindowEventType::GamepadRemoved:
                setupDefaultGamepad();

                break;

            case WindowEventType::GamepadAxisMotion:
                onGamepadMotionEvent(inEvent.data);

                break;

            case WindowEventType::GamepadButtonDown:
            case WindowEventType::GamepadButtonUp:
                onGamepadButtonEvent(inEvent.data);

                break;

            // Keyboard
            case WindowEventType::KeyboardAdded:
            case WindowEventType::KeyboardRemoved:
                setupDefaultKeyboard();

                break;

            case WindowEventType::KeyDown:
            case WindowEventType::KeyUp:
                onKeyboardButtonEvent(inEvent.data);

                break;

            // Mouse
            case WindowEventType::MouseAdded:
            case WindowEventType::MouseRemoved:
                setupDefaultMouse();

                break;

            case WindowEventType::MouseMotion:
                onMouseMotionEvent(inEvent.data);

                break;

            case WindowEventType::MouseButtonDown:
            case WindowEventType::MouseButtonUp:
                onMouseButtonEvent(inEvent.data);

                break;

            default:
                break;
            }

            onEvent(inEvent);
        });
    }

    void Controller::clearEvents()
    {
        m_mouseMotionEvents.clear();
        m_mouseButtonEvents.clear();

        m_keyboardKeyEvents.clear();

        m_gamepadButtonEvents.clear();
        m_gamepadMotionEvents.clear();
    }

    void Controller::setupDevices()
    {
        setupDefaultGamepad();
        setupDefaultKeyboard();
        setupDefaultMouse();
    }

    void Controller::setupDefaultGamepad()
    {
        disconnectFrom(Input::DeviceType::Gamepad);

        int             gamepadCount = 0;
        SDL_JoystickID* gamepadId    = SDL_GetGamepads(&gamepadCount);
        if (gamepadCount <= 0)
        {
            return;
        }

        connectTo(Input::DeviceType::Gamepad, *gamepadId);
    }

    void Controller::setupDefaultKeyboard()
    {
        disconnectFrom(Input::DeviceType::Keyboard);

        int             keyboardCount = 0;
        SDL_KeyboardID* keyboardId    = SDL_GetKeyboards(&keyboardCount);
        if (keyboardCount <= 0)
        {
            return;
        }

        for (int i = 0; i < keyboardCount; i++)
        {
            bool bIsDefault = false;
            for (const String& defaultName : DEFAULT_KEYBOARDS)
            {
                if (defaultName.equals(SDL_GetKeyboardNameForID(*keyboardId)))
                {
                    bIsDefault = true;

                    break;
                }
            }

            if (!bIsDefault)
            {
                break;
            }

            keyboardId++;
        }

        connectTo(Input::DeviceType::Keyboard, *keyboardId);
    }

    void Controller::setupDefaultMouse()
    {
        disconnectFrom(Input::DeviceType::Mouse);

        int          mouseCount = 0;
        SDL_MouseID* mouseId    = SDL_GetMice(&mouseCount);
        if (mouseCount <= 0)
        {
            return;
        }

        connectTo(Input::DeviceType::Mouse, *mouseId);
    }
}