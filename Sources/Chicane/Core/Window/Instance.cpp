#include "Chicane/Core/Window/Instance.hpp"

#include <SDL3/SDL.h>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Input.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

const Chicane::Vec<2, int> VEC2_ZERO(0);

namespace Chicane
{
    namespace Window
    {
        void setupGamepadDeviceEvent(Input::DeviceEvent& outEvent, const SDL_GamepadDeviceEvent& inData)
        {
            outEvent.device = inData.which;
        }

        void setupGamepadMotionEvent(Input::GamepadMotionEvent& outEvent, const SDL_GamepadAxisEvent& inData)
        {
            outEvent.device = inData.which;
            outEvent.axis   = (Input::GamepadAxis) inData.axis;
            outEvent.value  = static_cast<float>(inData.value) / 32767.0f;
        }

        void setupGamepadButtonEvent(Input::GamepadButtonEvent& outEvent, const SDL_GamepadButtonEvent& inData)
        {
            outEvent.device = inData.which;
            outEvent.button = (Input::GamepadButton) inData.button;
            outEvent.status = inData.down ? Input::Status::Pressed : Input::Status::Released;
        }

        void setupKeyboardDeviceEvent(Input::DeviceEvent& outEvent, const SDL_KeyboardDeviceEvent& inData)
        {
            outEvent.device = inData.which;
        }

        void setupKeyboardEvent(Input::KeyboardEvent& outEvent, const SDL_KeyboardEvent& inData)
        {
            outEvent.device   = inData.which;
            outEvent.button   = (Input::KeyboardButton) inData.scancode;
            outEvent.modifier = (Input::KeyboardButtonModifier) inData.mod;
            outEvent.status   = inData.down ? Input::Status::Pressed : Input::Status::Released;
        }

        void setupMouseDeviceEvent(Input::DeviceEvent& outEvent, const SDL_MouseDeviceEvent& inData)
        {
            outEvent.device = inData.which;
        }

        void setupMouseMotionEvent(Input::MouseMotionEvent& outEvent, const SDL_MouseMotionEvent& inData)
        {
            outEvent.device             = inData.which;
            outEvent.status             = (Input::Status) inData.state;
            outEvent.location.x         = inData.x;
            outEvent.location.y         = inData.y;
            outEvent.relativeLocation.x = inData.xrel;
            outEvent.relativeLocation.y = inData.yrel;
        }

        void setupMouseButtonEvent(Input::MouseButtonEvent& outEvent, const SDL_MouseButtonEvent& inData)
        {
            outEvent.device     = inData.which;
            outEvent.button     = (Input::MouseButton) inData.button;
            outEvent.status     = inData.down ? Input::Status::Pressed : Input::Status::Released;
            outEvent.clicks     = inData.clicks;
            outEvent.location.x = inData.x;
            outEvent.location.y = inData.y;
        }

        Instance::Instance()
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
            initFlags |= SDL_INIT_HAPTIC;
            initFlags |= SDL_INIT_AUDIO;
            initFlags |= SDL_INIT_VIDEO;

            if (!SDL_Init(initFlags))
            {
                emmitError("Error initing the window");
            }
        }

        Instance::~Instance()
        {
            destroy();

            SDL_Quit();
        }

        void Instance::init(const CreateInfo& inCreateInfo)
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

        bool Instance::run()
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
                case EventType::GamepadAdded:
                case EventType::GamepadRemoved:
                    setupGamepadDeviceEvent(deviceEvent, data.gdevice);
                    event.data = &deviceEvent;

                    break;

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
                case EventType::KeyboardAdded:
                case EventType::KeyboardRemoved:
                    setupKeyboardDeviceEvent(deviceEvent, data.kdevice);
                    event.data = &deviceEvent;

                    break;

                case EventType::KeyDown:
                case EventType::KeyUp:
                    setupKeyboardEvent(keyboardEvent, data.key);
                    event.data = &keyboardEvent;

                    break;

                // Mouse
                case EventType::MouseAdded:
                case EventType::MouseRemoved:
                    setupMouseDeviceEvent(deviceEvent, data.mdevice);
                    event.data = &deviceEvent;

                    break;

                case EventType::MouseMotion:
                    setupMouseMotionEvent(mouseMotionEvent, data.motion);
                    event.data = &mouseMotionEvent;

                    break;

                case EventType::MouseButtonDown:
                case EventType::MouseButtonUp:
                    setupMouseButtonEvent(mouseButtonEvent, data.button);
                    event.data = &mouseButtonEvent;

                    break;
                }

                m_eventObservable.next(event);
            }

            return true;
        }

        const Vec<2, int>& Instance::getSize() const
        {
            if (m_bIsMinimized)
            {
                return VEC2_ZERO;
            }

            return m_size;
        }

        void Instance::setSize(const Vec<2, int>& inValue)
        {
            setSize(inValue.x, inValue.y);
        }

        void Instance::setSize(int inWidth, int inHeight)
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

        const Vec<2, int>& Instance::getPosition() const
        {
            return m_position;
        }

        void Instance::setPosition(const Vec<2, int>& inValue)
        {
            setPosition(inValue.x, inValue.y);
        }

        void Instance::setPosition(int inX, int inY)
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

        void Instance::setTitle(const std::string& inTitle)
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

        void Instance::setIcon(const FileSystem::Path& inPath)
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

        void Instance::setDisplay(int inIndex)
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

        void Instance::setType(Type inType)
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

        Type Instance::getType() const
        {
            return m_type;
        }

        void* Instance::getInstance() const
        {
            return m_instance;
        }

        bool Instance::wasCreated() const
        {
            return m_instance != nullptr;
        }

        void Instance::destroy()
        {
            if (!wasCreated())
            {
                return;
            }

            SDL_DestroyWindow(static_cast<SDL_Window*>(m_instance));
        }

        bool Instance::isFocused() const
        {
            return m_bIsFocused;
        }

        void Instance::focus()
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

        void Instance::switchFocus()
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

        void Instance::blur()
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

        bool Instance::isResizable()
        {
            return m_bIsResizable;
        }

        void Instance::enableResizing()
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

        void Instance::disableResizing()
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

        bool Instance::isMinimized()
        {
            if (!wasCreated())
            {
                return m_bIsMinimized;
            }

            Vec<2, int> currentSize = getSize();

            return m_bIsMinimized || (currentSize.x <= 0.0f || currentSize.y <= 0.0f);
        }

        Instance::EventSubscription Instance::watchEvent(
            EventSubscription::NextCallback inNext,
            EventSubscription::ErrorCallback inError,
            EventSubscription::CompleteCallback inComplete
        )
        {
            return m_eventObservable.subscribe(inNext, inError, inComplete);
        }

        Instance::SizeSubscription Instance::watchSize(
            SizeSubscription::NextCallback inNext,
            SizeSubscription::ErrorCallback inError,
            SizeSubscription::CompleteCallback inComplete
        )
        {
            return m_sizeObservable
                .subscribe(inNext, inError, inComplete)
                .next(m_size);
        }

        void Instance::refreshSize()
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

        void Instance::refreshPosition()
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

        void Instance::emmitWarning(const std::string& inMessage)
        {
            if (inMessage.empty())
            {
                return;
            }

            std::string message = String::trim(inMessage);
            message.append(" [%s]");

            Log::warning(message, SDL_GetError());
        }

        void Instance::emmitError(const std::string& inMessage)
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
}