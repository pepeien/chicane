#include "Chicane/Core/Window.hpp"

#include <SDL3/SDL.h>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Gamepad/Button/Event.hpp"
#include "Chicane/Core/Input/Gamepad/Motion/Event.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    static inline const Vec<2, std::uint32_t> VEC2_ZERO(0);

    Window::Window()
        : m_instance(nullptr),
          m_settings({}),
          m_bIsFocused(false),
          m_bIsResizable(true),
          m_bIsMinimized(false),
          m_eventObservable({}),
          m_sizeObservable({}),
          m_backendObservable({})
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
        destroyInstance();

        SDL_Quit();
    }

    void Window::init(const WindowSettings& inSettings)
    {
        setBackend(inSettings.backend);
        setTitle(inSettings.title);
        setIcon(inSettings.icon);
        setSize(inSettings.size);
        setDisplay(inSettings.display);
        setType(inSettings.type);
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
                deviceEvent = Input::DeviceEvent::fromGamepad(&data.gdevice);
                event.data  = &deviceEvent;

                break;

            case WindowEventType::GamepadAxisMotion:
                gamepadMotionEvent = Input::GamepadMotionEvent(&data.gaxis);
                event.data         = &gamepadMotionEvent;

                break;

            case WindowEventType::GamepadButtonDown:
            case WindowEventType::GamepadButtonUp:
                gamepadButtonEvent = Input::GamepadButtonEvent(&data.gbutton);
                event.data         = &gamepadButtonEvent;

                break;

            // Keyboard
            case WindowEventType::KeyboardAdded:
            case WindowEventType::KeyboardRemoved:
                deviceEvent = Input::DeviceEvent::fromKeyboard(&data.kdevice);
                event.data  = &deviceEvent;

                break;

            case WindowEventType::KeyDown:
            case WindowEventType::KeyUp:
                keyboardEvent = Input::KeyboardEvent(&data.key);
                event.data    = &keyboardEvent;

                break;

            // Mouse
            case WindowEventType::MouseAdded:
            case WindowEventType::MouseRemoved:
                deviceEvent = Input::DeviceEvent::fromMouse(&data.mdevice);
                event.data  = &deviceEvent;

                break;

            case WindowEventType::MouseMotion:
                mouseMotionEvent = Input::MouseMotionEvent(&data.motion);
                event.data       = &mouseMotionEvent;

                break;

            case WindowEventType::MouseButtonDown:
            case WindowEventType::MouseButtonUp:
                mouseButtonEvent = Input::MouseButtonEvent(&data.button);
                event.data       = &mouseButtonEvent;

                break;

            default:
                break;
            }

            m_eventObservable.next(event);
        }

        return true;
    }

    void Window::restart()
    {
        destroyInstance();
        initInstance();

        setTitle(m_settings.title);
        setIcon(m_settings.icon);
        setSize(m_settings.size);
        setDisplay(m_settings.display);
        setType(m_settings.type);
    }

    const Vec<2, std::uint32_t>& Window::getSize() const
    {
        if (m_bIsMinimized)
        {
            return VEC2_ZERO;
        }

        return m_settings.size;
    }

    void Window::setSize(const Vec<2, std::uint32_t>& inValue)
    {
        setSize(inValue.x, inValue.y);
    }

    void Window::setSize(int inWidth, int inHeight)
    {
        if (!hasInstance())
        {
            return;
        }

        if (!SDL_SetWindowSize(static_cast<SDL_Window*>(m_instance), inWidth, inHeight))
        {
            emmitWarning("Failed to set the window size");

            return;
        }

        m_settings.size.x = inWidth;
        m_settings.size.y = inHeight;

        m_sizeObservable.next(m_settings.size);
    }

    const Vec<2, std::uint32_t>& Window::getPosition() const
    {
        return m_settings.position;
    }

    void Window::setPosition(const Vec<2, std::uint32_t>& inValue)
    {
        setPosition(inValue.x, inValue.y);
    }

    void Window::setPosition(int inX, int inY)
    {
        if (!hasInstance())
        {
            return;
        }

        if (!SDL_SetWindowPosition(static_cast<SDL_Window*>(m_instance), inX, inY))
        {
            emmitWarning("Failed to set the window position");

            return;
        }

        m_settings.position.x = inX;
        m_settings.position.y = inY;
    }

    void Window::setTitle(const String& inTitle)
    {
        if (!hasInstance())
        {
            return;
        }

        const String title = inTitle.trim();

        if (!SDL_SetWindowTitle(static_cast<SDL_Window*>(m_instance), title.toChar()))
        {
            emmitWarning("Failed to set the window title");

            return;
        }

        m_settings.title = title;
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
            SDL_PIXELFORMAT_RGBA8888,
            image.getPixels(),
            image.getPitch()
        );

        if (!icon || !SDL_SetWindowIcon(static_cast<SDL_Window*>(m_instance), icon))
        {
            emmitWarning("Failed to set the window icon");

            return;
        }

        m_settings.icon = inPath;

        SDL_DestroySurface(icon);
    }

    std::uint32_t Window::getDisplay() const
    {
        return m_settings.display;
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

        setSize(
            std::min(m_settings.size.x, static_cast<std::uint32_t>(displaySettings->w)),
            std::min(m_settings.size.y, static_cast<std::uint32_t>(displaySettings->h))
        );

        setPosition(SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
    }

    WindowType Window::getType() const
    {
        return m_settings.type;
    }

    void Window::setType(WindowType inType)
    {
        if (!hasInstance())
        {
            return;
        }

        m_settings.type = inType;

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

    WindowBackend Window::getBackend() const
    {
        return m_settings.backend;
    }

    void Window::setBackend(WindowBackend inBackend)
    {
        if (m_settings.backend == inBackend)
        {
            return;
        }

        m_settings.backend = inBackend;

        if (!hasInstance())
        {
            initInstance();
        }
        else
        {
            restart();
        }

        m_backendObservable.next(inBackend);
    }

    bool Window::hasInstance() const
    {
        return m_instance != nullptr;
    }

    void Window::initInstance()
    {
        if (hasInstance())
        {
            return;
        }

        SDL_WindowFlags flag = 0;

        switch (m_settings.backend)
        {
#if CHICANE_OPENGL
        case WindowBackend::OpenGL:
            flag = SDL_WINDOW_OPENGL;

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            if (IS_DEBUGGING)
            {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            }

            break;
#endif

#if CHICANE_VULKAN
        case WindowBackend::Vulkan:
            flag = SDL_WINDOW_VULKAN;

            break;
#endif

        default:
            flag = SDL_WINDOW_HIDDEN;

            break;
        };

        m_instance = SDL_CreateWindow("", 0, 0, flag);

        if (!m_instance)
        {
            emmitError("Error creating window");
        }
    }

    void* Window::getInstance() const
    {
        return m_instance;
    }

    void Window::destroyInstance()
    {
        if (!hasInstance())
        {
            return;
        }

        SDL_DestroyWindow(static_cast<SDL_Window*>(m_instance));
        m_instance = nullptr;
    }

    bool Window::isFocused() const
    {
        return m_bIsFocused;
    }

    void Window::focus()
    {
        if (!hasInstance())
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
        if (!hasInstance())
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
        if (!hasInstance())
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

    bool Window::isResizable() const
    {
        return m_bIsResizable;
    }

    void Window::enableResizing()
    {
        if (!hasInstance())
        {
            return;
        }

        if (m_settings.type != WindowType::Windowed)
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
        if (!hasInstance())
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

    bool Window::isMinimized() const
    {
        if (!hasInstance())
        {
            return m_bIsMinimized;
        }

        Vec<2, std::uint32_t> currentSize = getSize();

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
        return m_sizeObservable.subscribe(inNext, inError, inComplete).next(m_settings.size);
    }

    WindowBackendSubscription Window::watchBackend(
        WindowBackendSubscription::NextCallback     inNext,
        WindowBackendSubscription::ErrorCallback    inError,
        WindowBackendSubscription::CompleteCallback inComplete
    )
    {
        return m_backendObservable.subscribe(inNext, inError, inComplete).next(m_settings.backend);
    }

    void Window::refreshSize()
    {
        if (!hasInstance())
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

        m_settings.size.x = width;
        m_settings.size.y = height;

        m_sizeObservable.next(m_settings.size);
    }

    void Window::refreshPosition()
    {
        if (!hasInstance())
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

        m_settings.position.x = x;
        m_settings.position.y = y;
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