#include "Chicane/Core/Window.hpp"

#include <SDL3/SDL.h>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Gamepad/ButtonEvent.hpp"
#include "Chicane/Core/Input/Gamepad/MotionEvent.hpp"
#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/ButtonEvent.hpp"
#include "Chicane/Core/Input/Mouse/MotionEvent.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    const Chicane::Vec<2, int> VEC2_ZERO(0);

    void setupGamepadDeviceEvent(Input::DeviceEvent& outEvent, const SDL_GamepadDeviceEvent& inData)
    {
        outEvent.device = inData.which;
    }

    void setupGamepadMotionEvent(
        Input::GamepadMotionEvent& outEvent, const SDL_GamepadAxisEvent& inData
    )
    {
        outEvent.device = inData.which;
        outEvent.axis   = (Input::GamepadAxis)inData.axis;
        outEvent.value  = static_cast<float>(inData.value) / 32767.0f;
    }

    void setupGamepadButtonEvent(
        Input::GamepadButtonEvent& outEvent, const SDL_GamepadButtonEvent& inData
    )
    {
        outEvent.device = inData.which;
        outEvent.button = (Input::GamepadButton)inData.button;
        outEvent.status = inData.down ? Input::Status::Pressed : Input::Status::Released;
    }

    void setupKeyboardDeviceEvent(
        Input::DeviceEvent& outEvent, const SDL_KeyboardDeviceEvent& inData
    )
    {
        outEvent.device = inData.which;
    }

    void setupKeyboardEvent(Input::KeyboardEvent& outEvent, const SDL_KeyboardEvent& inData)
    {
        outEvent.device   = inData.which;
        outEvent.button   = (Input::KeyboardButton)inData.scancode;
        outEvent.modifier = (Input::KeyboardButtonModifier)inData.mod;
        outEvent.status   = inData.down ? Input::Status::Pressed : Input::Status::Released;
    }

    void setupMouseDeviceEvent(Input::DeviceEvent& outEvent, const SDL_MouseDeviceEvent& inData)
    {
        outEvent.device = inData.which;
    }

    void setupMouseMotionEvent(
        Input::MouseMotionEvent& outEvent, const SDL_MouseMotionEvent& inData
    )
    {
        outEvent.device             = inData.which;
        outEvent.status             = (Input::Status)inData.state;
        outEvent.location.x         = inData.x;
        outEvent.location.y         = inData.y;
        outEvent.relativeLocation.x = inData.xrel;
        outEvent.relativeLocation.y = inData.yrel;
    }

    void setupMouseButtonEvent(
        Input::MouseButtonEvent& outEvent, const SDL_MouseButtonEvent& inData
    )
    {
        outEvent.device     = inData.which;
        outEvent.button     = (Input::MouseButton)inData.button;
        outEvent.status     = inData.down ? Input::Status::Pressed : Input::Status::Released;
        outEvent.clicks     = inData.clicks;
        outEvent.location.x = inData.x;
        outEvent.location.y = inData.y;
    }

    Window::Window()
        : m_instance(nullptr),
          m_title(""),
          m_icon(""),
          m_size(Vec<2, int>(0)),
          m_display(0),
          m_type(WindowType::Fullscreen),
          m_position({}),
          m_bIsFocused(false),
          m_bIsResizable(true),
          m_bIsMinimized(false),
          m_sizeObservable({})
    {
        SDL_InitFlags initFlags = 0;
        initFlags |= SDL_INIT_GAMEPAD;
        initFlags |= SDL_INIT_HAPTIC;
        initFlags |= SDL_INIT_VIDEO;

        if (!SDL_Init(initFlags))
        {
            emmitError("Error initing the window");
        }
    }

    Window::~Window()
    {
        destroy();

        SDL_Quit();
    }

    void Window::init(const WindowCreateInfo& inCreateInfo)
    {
        SDL_WindowFlags flag = 0;

        switch (inCreateInfo.renderer)
        {
        case WindowRenderer::OpenGL:
            flag = SDL_WINDOW_OPENGL;

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            break;

        case WindowRenderer::Vulkan:
            flag = SDL_WINDOW_VULKAN;

            break;

        default:
            flag = SDL_WINDOW_HIDDEN;

            break;
        };

        if (wasCreated())
        {
            destroy();
        }

        m_instance = SDL_CreateWindow("", 0, 0, flag);

        if (!m_instance)
        {
            emmitError("Error creating window");
        }

        setTitle(inCreateInfo.title);
        setIcon(inCreateInfo.icon);
        setSize(inCreateInfo.size);
        setDisplay(inCreateInfo.display);
        setType(inCreateInfo.type);
    }

    bool Window::run()
    {
        SDL_Event data;

        while (SDL_PollEvent(&data))
        {
            // Device
            Input::DeviceEvent deviceEvent = {};

            // Gamepad
            Input::GamepadMotionEvent gamepadMotionEvent = {};
            Input::GamepadButtonEvent gamepadButtonEvent = {};

            // Keyboard
            Input::KeyboardEvent keyboardEvent = {};

            // Mouse
            Input::MouseMotionEvent mouseMotionEvent = {};
            Input::MouseButtonEvent mouseButtonEvent = {};

            // Event
            WindowEvent event = {};
            event.type        = (WindowEventType)data.type;
            event.data        = nullptr;

            switch (event.type)
            {
            case WindowEventType::Quit:
                return false;

            case WindowEventType::WindowShown:
                refreshSize();
                refreshPosition();

                break;

            case WindowEventType::WindowExposed:
                m_bIsMinimized = false;

                refreshSize();

                break;

            case WindowEventType::WindowMinimized:
                m_bIsMinimized = true;

                break;

            case WindowEventType::WindowResized:
                refreshSize();

                break;

            case WindowEventType::WindowDisplayChanged:
            case WindowEventType::WindowMoved:
                refreshPosition();

                break;

            // Gamepad
            case WindowEventType::GamepadAdded:
            case WindowEventType::GamepadRemoved:
                setupGamepadDeviceEvent(deviceEvent, data.gdevice);
                event.data = &deviceEvent;

                break;

            case WindowEventType::GamepadAxisMotion:
                setupGamepadMotionEvent(gamepadMotionEvent, data.gaxis);
                event.data = &gamepadMotionEvent;

                break;

            case WindowEventType::GamepadButtonDown:
            case WindowEventType::GamepadButtonUp:
                setupGamepadButtonEvent(gamepadButtonEvent, data.gbutton);
                event.data = &gamepadButtonEvent;

                break;

            // Keyboard
            case WindowEventType::KeyboardAdded:
            case WindowEventType::KeyboardRemoved:
                setupKeyboardDeviceEvent(deviceEvent, data.kdevice);
                event.data = &deviceEvent;

                break;

            case WindowEventType::KeyDown:
            case WindowEventType::KeyUp:
                setupKeyboardEvent(keyboardEvent, data.key);
                event.data = &keyboardEvent;

                break;

            // Mouse
            case WindowEventType::MouseAdded:
            case WindowEventType::MouseRemoved:
                setupMouseDeviceEvent(deviceEvent, data.mdevice);
                event.data = &deviceEvent;

                break;

            case WindowEventType::MouseMotion:
                setupMouseMotionEvent(mouseMotionEvent, data.motion);
                event.data = &mouseMotionEvent;

                break;

            case WindowEventType::MouseButtonDown:
            case WindowEventType::MouseButtonUp:
                setupMouseButtonEvent(mouseButtonEvent, data.button);
                event.data = &mouseButtonEvent;

                break;

            default:
                break;
            }

            m_eventObservable.next(event);
        }

        return true;
    }

    const Vec<2, int>& Window::getSize() const
    {
        if (m_bIsMinimized)
        {
            return VEC2_ZERO;
        }

        return m_size;
    }

    void Window::setSize(const Vec<2, int>& inValue)
    {
        setSize(inValue.x, inValue.y);
    }

    void Window::setSize(int inWidth, int inHeight)
    {
        if (!wasCreated())
        {
            return;
        }

        if (!SDL_SetWindowSize(static_cast<SDL_Window*>(m_instance), inWidth, inHeight))
        {
            emmitWarning("Failed to set the window size");

            return;
        }

        m_size.x = inWidth;
        m_size.y = inHeight;

        m_sizeObservable.next(m_size);
    }

    const Vec<2, int>& Window::getPosition() const
    {
        return m_position;
    }

    void Window::setPosition(const Vec<2, int>& inValue)
    {
        setPosition(inValue.x, inValue.y);
    }

    void Window::setPosition(int inX, int inY)
    {
        if (!wasCreated())
        {
            return;
        }

        if (!SDL_SetWindowPosition(static_cast<SDL_Window*>(m_instance), inX, inY))
        {
            emmitWarning("Failed to set the window position");

            return;
        }

        m_position.x = inX;
        m_position.y = inY;
    }

    void Window::setTitle(const String& inTitle)
    {
        if (!wasCreated())
        {
            return;
        }

        const String title = inTitle.trim();

        if (!SDL_SetWindowTitle(static_cast<SDL_Window*>(m_instance), title.toChar()))
        {
            emmitWarning("Failed to set the window title");

            return;
        }

        m_title = title;
    }

    void Window::setIcon(const FileSystem::Path& inPath)
    {
        if (inPath.empty())
        {
            return;
        }

        if (!FileSystem::exists(inPath))
        {
            emmitError("The window icon [ " + inPath.string() + " ] doesn't exists");
        }

        const Image image = Image(inPath);

        SDL_Surface* icon = SDL_CreateSurfaceFrom(
            image.getWidth(),
            image.getHeight(),
            SDL_PIXELFORMAT_ARGB8888,
            image.getPixels(),
            image.getPitch()
        );

        if (!icon || !SDL_SetWindowIcon(static_cast<SDL_Window*>(m_instance), icon))
        {
            emmitWarning("Failed to set the window icon");

            return;
        }

        m_icon = inPath;

        SDL_DestroySurface(icon);
    }

    void Window::setDisplay(std::uint32_t inIndex)
    {
        int            displayCount = 0;
        SDL_DisplayID* displays     = SDL_GetDisplays(&displayCount);

        int display = displays[std::min(static_cast<int>(inIndex), displayCount - 1)];

        const SDL_DisplayMode* displaySettings = SDL_GetCurrentDisplayMode(display);

        SDL_free(displays);

        if (!displaySettings)
        {
            emmitError("Error while setting the window display");
        }

        setSize(std::min(m_size.x, displaySettings->w), std::min(m_size.y, displaySettings->h));

        setPosition(
            SDL_WINDOWPOS_CENTERED_DISPLAY(display),
            SDL_WINDOWPOS_CENTERED_DISPLAY(display)
        );
    }

    void Window::setType(WindowType inType)
    {
        if (!wasCreated())
        {
            return;
        }

        m_type = inType;

        switch (inType)
        {
        case WindowType::Windowed:
            SDL_SetWindowBordered(static_cast<SDL_Window*>(m_instance), true);

            if (m_bIsResizable)
            {
                enableResizing();

                break;
            }

            disableResizing();

            break;

        case WindowType::WindowedBorderless:
            SDL_SetWindowBordered(static_cast<SDL_Window*>(m_instance), false);

            disableResizing();

            break;

        case WindowType::Fullscreen:
            SDL_SetWindowFullscreen(static_cast<SDL_Window*>(m_instance), true);

            disableResizing();

            break;
        }
    }

    WindowType Window::getType() const
    {
        return m_type;
    }

    void* Window::getInstance() const
    {
        return m_instance;
    }

    bool Window::wasCreated() const
    {
        return m_instance != nullptr;
    }

    void Window::destroy()
    {
        if (!wasCreated())
        {
            return;
        }

        SDL_DestroyWindow(static_cast<SDL_Window*>(m_instance));
    }

    bool Window::isFocused() const
    {
        return m_bIsFocused;
    }

    void Window::focus()
    {
        if (!wasCreated())
        {
            return;
        }

        if (!SDL_SetWindowRelativeMouseMode(static_cast<SDL_Window*>(m_instance), true))
        {
            emmitWarning("Failed to focus the window");

            return;
        }

        m_bIsFocused = true;
    }

    void Window::switchFocus()
    {
        if (!wasCreated())
        {
            return;
        }

        if (isFocused())
        {
            blur();
        }
        else
        {
            focus();
        }
    }

    void Window::blur()
    {
        if (!wasCreated())
        {
            return;
        }

        if (!SDL_SetWindowRelativeMouseMode(static_cast<SDL_Window*>(m_instance), false))
        {
            emmitWarning("Failed to blur the window");

            return;
        }

        m_bIsFocused = false;
    }

    bool Window::isResizable()
    {
        return m_bIsResizable;
    }

    void Window::enableResizing()
    {
        if (!wasCreated())
        {
            return;
        }

        if (m_type != WindowType::Windowed)
        {
            return;
        }

        if (!SDL_SetWindowResizable(static_cast<SDL_Window*>(m_instance), true))
        {
            emmitWarning("Failed to enable window resizing");

            return;
        }

        m_bIsResizable = true;
    }

    void Window::disableResizing()
    {
        if (!wasCreated())
        {
            return;
        }

        if (!SDL_SetWindowResizable(static_cast<SDL_Window*>(m_instance), false))
        {
            emmitWarning("Failed to disable window resizing");

            return;
        }

        m_bIsResizable = false;
    }

    bool Window::isMinimized()
    {
        if (!wasCreated())
        {
            return m_bIsMinimized;
        }

        Vec<2, int> currentSize = getSize();

        return m_bIsMinimized || (currentSize.x <= 0.0f || currentSize.y <= 0.0f);
    }

    WindowEventSubscription Window::watchEvent(
        WindowEventSubscription::NextCallback     inNext,
        WindowEventSubscription::ErrorCallback    inError,
        WindowEventSubscription::CompleteCallback inComplete
    )
    {
        return m_eventObservable.subscribe(inNext, inError, inComplete);
    }

    WindowSizeSubscription Window::watchSize(
        WindowSizeSubscription::NextCallback     inNext,
        WindowSizeSubscription::ErrorCallback    inError,
        WindowSizeSubscription::CompleteCallback inComplete
    )
    {
        return m_sizeObservable.subscribe(inNext, inError, inComplete).next(m_size);
    }

    void Window::refreshSize()
    {
        if (!wasCreated())
        {
            return;
        }

        int width  = 0;
        int height = 0;

        if (!SDL_GetWindowSize(static_cast<SDL_Window*>(m_instance), &width, &height))
        {
            emmitWarning("Failed to refresh the window size");

            return;
        }

        m_size.x = width;
        m_size.y = height;

        m_sizeObservable.next(m_size);
    }

    void Window::refreshPosition()
    {
        if (!wasCreated())
        {
            return;
        }

        int x = 0;
        int y = 0;

        if (!SDL_GetWindowPosition(static_cast<SDL_Window*>(m_instance), &x, &y))
        {
            emmitWarning("Failed to refresh the window position");

            return;
        }

        m_position.x = x;
        m_position.y = y;
    }

    void Window::emmitWarning(const String& inMessage)
    {
        if (inMessage.isEmpty())
        {
            return;
        }

        String message = inMessage.trim();
        message.append(" [%s]");

        Log::warning(message, SDL_GetError());
    }

    void Window::emmitError(const String& inMessage)
    {
        if (inMessage.isEmpty())
        {
            throw std::runtime_error(SDL_GetError());
        }

        String message = inMessage.trim();
        message.append(" [%s]");

        throw std::runtime_error(String::sprint(message, SDL_GetError()).toChar());
    }
}