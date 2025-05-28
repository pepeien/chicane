#include "Chicane/Core/Window.hpp"

#include <SDL3/SDL.h>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Input.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

const Chicane::Vec<2, int> VEC2_ZERO(0);

namespace Chicane
{
    void setupGamepadMotionEvent(Gamepad::MotionEvent& outEvent, const SDL_GamepadAxisEvent& inData)
    {
        outEvent.axis  = (Gamepad::Axis) inData.axis;
        outEvent.value = inData.value;
    }

    void setupGamepadButtonEvent(Gamepad::ButtonEvent& outEvent, const SDL_GamepadButtonEvent& inData)
    {
        outEvent.button = (Gamepad::Button) inData.button;
        outEvent.status = inData.down ? Input::Status::Pressed : Input::Status::Released; 
    }

    void setupKeyboardEvent(Keyboard::Event& outEvent, const SDL_KeyboardEvent& inData)
    {
        outEvent.button   = (Keyboard::Button) inData.scancode;
        outEvent.modifier = (Keyboard::ButtonModifier) inData.mod;
        outEvent.status   = inData.down ? Input::Status::Pressed : Input::Status::Released; 
    }

    void setupMouseMotionEvent(Mouse::MotionEvent& outEvent, const SDL_MouseMotionEvent& inData)
    {
       outEvent.status             = (Input::Status) inData.state;
       outEvent.location.x         = inData.x;
       outEvent.location.y         = inData.y;
       outEvent.relativeLocation.x = inData.xrel;
       outEvent.relativeLocation.y = inData.yrel;
    }

    void setupMouseButtonEvent(Mouse::ButtonEvent& outEvent, const SDL_MouseButtonEvent& inData)
    {
        outEvent.button     = (Mouse::Button) inData.button;
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
        m_type(Type::Fullscreen),
        m_position({}),
        m_bIsFocused(false),
        m_bIsResizable(true),
        m_bIsMinimized(false),
        m_sizeObservable({})
    {
        SDL_InitFlags initFlags = 0;
        initFlags |= SDL_INIT_GAMEPAD;
        initFlags |= SDL_INIT_VIDEO;
        initFlags |= SDL_INIT_AUDIO;

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

    void Window::init(const CreateInfo& inCreateInfo)
    {
        SDL_WindowFlags flag = 0;

        switch (inCreateInfo.renderer)
        {
        case Renderer::OpenGL:
            flag = SDL_WINDOW_OPENGL;

            break;

        case Renderer::Vulkan:
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
            // Gamepad
            Gamepad::MotionEvent gamepadMotionEvent = {};
            Gamepad::ButtonEvent gamepadButtonEvent = {};

            // Keyboard
            Keyboard::Event keyboardEvent = {};

            // Mouse
            Mouse::MotionEvent mouseMotionEvent = {};
            Mouse::ButtonEvent mouseButtonEvent = {};

            // Event
            Event event = {};
            event.type = (EventType) data.type;
            event.data = nullptr;

            switch (event.type)
            {
            case EventType::Quit:
                return false;

            case EventType::WindowShown:
                refreshSize();
                refreshPosition();

                break;

            case EventType::WindowExposed:
                m_bIsMinimized = false;

                refreshSize();

                break;

            case EventType::WindowMinimized:
                m_bIsMinimized = true;

                break;

            case EventType::WindowResized:
                refreshSize();

                break;

            case EventType::WindowDisplayChanged:
            case EventType::WindowMoved:
                refreshPosition();

                break;

            // Gamepad
            case EventType::GamepadAxisMotion:
                setupGamepadMotionEvent(gamepadMotionEvent, data.gaxis);
                event.data = &gamepadMotionEvent;

                break;

            case EventType::GamepadButtonDown:
            case EventType::GamepadButtonUp:
                setupGamepadButtonEvent(gamepadButtonEvent, data.gbutton);
                event.data = &gamepadButtonEvent;

                break;

            // Keyboard
            case EventType::KeyDown:
            case EventType::KeyUp:
                setupKeyboardEvent(keyboardEvent, data.key);
                event.data = &keyboardEvent;

                break;

            // Mouse
            case Window::EventType::MouseMotion:
                setupMouseMotionEvent(mouseMotionEvent, data.motion);
                event.data = &mouseMotionEvent;

                break;

            case Window::EventType::MouseButtonDown:
            case Window::EventType::MouseButtonUp:
                setupMouseButtonEvent(mouseButtonEvent, data.button);
                event.data = &mouseButtonEvent;

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

    void Window::setTitle(const std::string& inTitle)
    {
        if (!wasCreated())
        {
            return;
        }

        const std::string title = String::trim(inTitle);

        if (!SDL_SetWindowTitle(static_cast<SDL_Window*>(m_instance), title.c_str()))
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

        const Image::Instance image = Image::Instance(inPath);

        SDL_Surface* icon = SDL_CreateSurfaceFrom(
            image.getWidth(),
            image.getHeight(),
            SDL_PIXELFORMAT_ARGB8888,
            image.getPixels(),
            image.getPitch()
        );

        if (
            !icon ||
            !SDL_SetWindowIcon(static_cast<SDL_Window*>(m_instance), icon)
        )
        {
            emmitWarning("Failed to set the window icon");

            return;
        }

        m_icon = inPath;

        SDL_DestroySurface(icon);
    }

    void Window::setDisplay(int inIndex)
    {
        int displayCount = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
        int display = SDL_GetDisplays(&displayCount)[std::min(inIndex, displayCount)];

        const SDL_DisplayMode* displaySettings = SDL_GetCurrentDisplayMode(display);

        SDL_free(displays);

        if (!displaySettings)
        {
            emmitError("Error while setting the window display");
        }

        setSize(
            std::min(m_size.x, displaySettings->w),
            std::min(m_size.y, displaySettings->h)
        );

        setPosition(
            SDL_WINDOWPOS_CENTERED_DISPLAY(inIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(inIndex)
        );
    }

    void Window::setType(Type inType)
    {
        if (!wasCreated())
        {
            return;
        }

        m_type = inType;

        switch (inType)
        {
        case Type::Windowed:
            SDL_SetWindowBordered(static_cast<SDL_Window*>(m_instance), true);

            if (m_bIsResizable)
            {
                enableResizing();

                break;
            }

            disableResizing();

            break;

        case Type::WindowedBorderless:
            SDL_SetWindowBordered(static_cast<SDL_Window*>(m_instance), false);

            disableResizing();

            break;

        case Type::Fullscreen:
            SDL_SetWindowFullscreen(static_cast<SDL_Window*>(m_instance), true);

            disableResizing();

            break;
        }
    }

    Window::Type Window::getType() const
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

        if (m_type != Type::Windowed)
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

    Window::EventSubscription Window::watchEvent(
        EventSubscription::NextCallback inNext,
        EventSubscription::ErrorCallback inError,
        EventSubscription::CompleteCallback inComplete
    )
    {
        return m_eventObservable.subscribe(inNext, inError, inComplete);
    }

    Window::SizeSubscription Window::watchSize(
        SizeSubscription::NextCallback inNext,
        SizeSubscription::ErrorCallback inError,
        SizeSubscription::CompleteCallback inComplete
    )
    {
        return m_sizeObservable
            .subscribe(inNext, inError, inComplete)
            .next(m_size);
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

        if (m_size.x == width && m_size.y == height)
        {
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

    void Window::emmitWarning(const std::string& inMessage)
    {
        if (inMessage.empty())
        {
            return;
        }

        std::string message = String::trim(inMessage);
        message.append(" [%s]");

        Log::warning(message, SDL_GetError());
    }

    void Window::emmitError(const std::string& inMessage)
    {
        if (inMessage.empty())
        {
            throw std::runtime_error(SDL_GetError());
        }

        std::string message = String::trim(inMessage);
        message.append(" [%s]");

        throw std::runtime_error(String::sprint(message, SDL_GetError()));
    }
}