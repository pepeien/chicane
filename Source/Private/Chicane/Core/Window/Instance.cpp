#include "Chicane/Core/Window/Instance.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

const Chicane::Vec<2, int> VEC2_ZERO(0);

namespace Chicane
{
    namespace Window
    {
        Instance::Instance(const CreateInfo& inCreateInfo)
            : instance(nullptr),
            m_bIsFocused(false),
            m_bIsResizable(true),
            m_bIsMinimized(false)
        {
            SDL_InitFlags initFlags = SDL_INIT_GAMEPAD;
            initFlags              |= SDL_INIT_VIDEO;
            initFlags              |= SDL_INIT_AUDIO;

            if (!SDL_InitSubSystem(initFlags))
            {
                throw std::runtime_error(SDL_GetError());
            }

            int displayCount = 0;
            SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
            int display = SDL_GetDisplays(&displayCount)[
                std::min(
                    inCreateInfo.display,
                    displayCount
                )
            ];

            const SDL_DisplayMode* displaySettings = SDL_GetCurrentDisplayMode(display);

            SDL_free(displays);

            if (!displaySettings)
            {
                throw std::runtime_error(SDL_GetError());
            }

            int width = std::min(
                inCreateInfo.resolution.x,
                displaySettings->w
            );
            int height = std::min(
                inCreateInfo.resolution.y,
                displaySettings->h
            );

            if (width <= 0 || height <= 0)
            {
                width  = displaySettings->w;
                height = displaySettings->h;
            }

            SDL_WindowFlags windowFlags = SDL_WINDOW_HIDDEN;

            if (inCreateInfo.renderer == Renderer::Type::Vulkan)
            {
                windowFlags = SDL_WINDOW_VULKAN;
            }

            instance = SDL_CreateWindow(
                inCreateInfo.title.c_str(),
                width,
                height,
                windowFlags
            );

            if (!instance)
            {
                throw std::runtime_error(SDL_GetError());
            }

            setDisplay(display);
            setType(inCreateInfo.window);
            setIcon(inCreateInfo.icon);

            refreshSize();
            refreshPosition();
        }

        Instance::~Instance()
        {
            SDL_DestroyWindow(instance);
            SDL_Quit();
        }

        void Instance::onEvent(const SDL_Event& inEvent)
        {
            switch (inEvent.type)
            {
            case SDL_EVENT_WINDOW_SHOWN:
                refreshSize();
                refreshPosition();

                break;

            case SDL_EVENT_WINDOW_EXPOSED:
                m_bIsMinimized = false;

                refreshSize();

                break;

            case SDL_EVENT_WINDOW_MINIMIZED:
                m_bIsMinimized = true;

                break;

            case SDL_EVENT_WINDOW_RESIZED:
                refreshSize();

                break;

            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            case SDL_EVENT_WINDOW_MOVED:
                refreshPosition();

                break;
            }
        }

        bool Instance::isFocused() const
        {
            return m_bIsFocused;
        }

        void Instance::focus()
        {
            m_bIsFocused = true;

            SDL_SetWindowRelativeMouseMode(instance, true);
        }

        void Instance::switchFocus()
        {
            if (isFocused())
            {
                blur();

                return;
            }

            focus();
        }

        void Instance::blur()
        {
            m_bIsFocused = false;

            SDL_SetWindowRelativeMouseMode(instance, false);
        }

        const Vec<2, int>& Instance::getSize() const
        {
            if (m_bIsMinimized)
            {
                return VEC2_ZERO;
            }

            return m_size;
        }

        void Instance::setSize(const Vec<2, int>& inSize)
        {
            setSize(inSize.x, inSize.y);
        }

        void Instance::setSize(int inWidth, int inHeight)
        {
            if (m_size.x == inWidth && m_size.y == inWidth)
            {
                return;
            }

            SDL_SetWindowSize(
                instance,
                inWidth,
                inHeight
            );

            m_size.x = inWidth;
            m_size.y = inHeight;
        }

        const Vec<2, int>& Instance::getDrawableSize() const
        {
            if (m_bIsMinimized)
            {
                return VEC2_ZERO;
            }

            return m_drawableSize;
        }

        void Instance::setDrawableSize(const Vec<2, int>& inSize)
        {
            setDrawableSize(
                inSize.x,
                inSize.y
            );
        }

        void Instance::setDrawableSize(int inWidth, int inHeight)
        {
            if (m_drawableSize.x == inWidth && m_drawableSize.y == inWidth)
            {
                return;
            }

            m_drawableSize.x = inWidth;
            m_drawableSize.y = inHeight;
        }

        const Vec<2, int>& Instance::getPosition() const
        {
            return m_position;
        }

        void Instance::setPosition(const Vec<2, int>& inPosition)
        {
            setPosition(
                inPosition.x,
                inPosition.y
            );
        }

        void Instance::setPosition(int inX, int inY)
        {
            if (m_position.x == inX && m_position.y == inX)
            {
                return;
            }

            SDL_SetWindowPosition(
                instance,
                inX,
                inY
            );

            m_position.x = inX;
            m_position.y = inY;
        }

        void Instance::setTitle(const std::string& inTitle)
        {
            SDL_SetWindowTitle(
                instance,
                inTitle.c_str()
            );
        }

        void Instance::setIcon(const std::string& inIconPath)
        {
            std::string iconPath = Utils::trim(inIconPath);

            if (iconPath.empty())
            {
                return;
            }

            if (!FileSystem::exists(iconPath))
            {
                throw std::runtime_error("The file [ " + iconPath + " ] doesn't exist");
            }

            int width   = 0;
            int height  = 0;
            int channel = 0;
            int format  = 0;

            Image::Pixels pixels = FileSystem::readImageFromFile(
                width,
                height,
                channel,
                format,
                inIconPath
            );

            SDL_Surface* icon = SDL_CreateSurfaceFrom(
                width,
                height,
                SDL_PIXELFORMAT_ARGB8888,
                pixels,
                format * width
            );

            if (!icon)
            {
                throw std::runtime_error(SDL_GetError());
            }

            bool iconResult = SDL_SetWindowIcon(instance, icon);

            SDL_DestroySurface(icon);
            stbi_image_free(pixels);

            if (!iconResult)
            {
                throw std::runtime_error(SDL_GetError());
            }
        }

        void Instance::setDisplay(int inMonitorIndex)
        {
            SDL_SetWindowPosition(
                instance,
                SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex),
                SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex)
            );
        }

        void Instance::setType(Type inType)
        {
            m_type = inType;

            switch (inType)
            {
            case Type::Windowed:
                SDL_SetWindowBordered(instance, true);

                if (m_bIsResizable)
                {
                    enableResizing();

                    break;
                }

                disableResizing();

                break;

            case Type::WindowedBorderless:
                SDL_SetWindowBordered(instance, false);

                disableResizing();

                break;

            case Type::Fullscreen:
                SDL_SetWindowFullscreen(instance, true);

                disableResizing();

                break;
            }
        }

        Type Instance::getType() const
        {
            return m_type;
        }

        bool Instance::isResizable()
        {
            return m_bIsResizable;
        }

        void Instance::enableResizing()
        {
            if (m_type != Type::Windowed)
            {
                return;
            }

            SDL_SetWindowResizable(
                instance,
                true
            );

            m_bIsResizable = true;
        }

        void Instance::disableResizing()
        {
            SDL_SetWindowResizable(
                instance,
                false
            );

            m_bIsResizable = false;
        }

        bool Instance::isMinimized()
        {
            Vec<2, int> currentSize = getSize();

            return m_bIsMinimized || (currentSize.x <= 0.0f || currentSize.y <= 0.0f);
        }

        void Instance::refreshSize()
        {
            int width  = 0;
            int height = 0;

            SDL_GetWindowSize(
                instance,
                &width,
                &height
            );

            setSize(width, height);

            refreshDrawableSize();
        }

        void Instance::refreshDrawableSize()
        {
            int width  = 0;
            int height = 0;

            SDL_GetWindowSize(
                instance,
                &width,
                &height
            );

            setDrawableSize(width, height);
        }

        void Instance::refreshPosition()
        {
            int x = 0;
            int y = 0;

            SDL_GetWindowPosition(
                instance,
                &x,
                &y
            );

            setPosition(x, y);
        }
    }
}