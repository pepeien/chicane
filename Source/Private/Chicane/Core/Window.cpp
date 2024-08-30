#include "Chicane/Core/Window.hpp"

#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"

const Chicane::Vec<2, int> VEC2_ZERO(0);

namespace Chicane
{
    Window::Window(const WindowCreateInfo& inCreateInfo)
        : instance(nullptr),
        m_isFocused(inCreateInfo.isFocused),
        m_isResizable(inCreateInfo.isResizable),
        m_isMinimized(false),
        m_renderer(nullptr)
    {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            throw std::runtime_error(SDL_GetError());
        }

        int displayCount;
        SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);

        int displayIndex = std::min(
            inCreateInfo.displayIndex,
            displayCount
        );

        const SDL_DisplayMode* displaySettings = SDL_GetCurrentDisplayMode(displays[displayIndex]);

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

        instance = SDL_CreateWindow(
            inCreateInfo.title.c_str(),
            width,
            height,
            SDL_WINDOW_VULKAN
        );

        if (!instance)
        {
            throw std::runtime_error(SDL_GetError());
        }

        setDisplay(displayIndex);
        setType(inCreateInfo.type);
        setIcon(inCreateInfo.icon);
        setWindow(this);

        refreshSize();
        refreshPosition();

        initRenderer();
        initCoreLayers();
        
    }

    Window::~Window()
    {
        SDL_DestroyWindow(instance);
        SDL_Quit();
    }

    const Renderer::Internals& Window::getRendererInternals()
    {
        return m_rendererInternals;
    }

    const Telemetry& Window::getTelemetry() const
    {
        return m_telemetry;
    }

    void Window::addLayer(
        Layer* inLayer,
        Layer::Push inPushTechnique,
        const std::string& inId
    )
    {
        switch (inPushTechnique)
        {
        case Layer::Push::Front:
            m_renderer->pushLayerStart(inLayer);

            return;

        case Layer::Push::BeforeLayer:
            m_renderer->pushLayerBefore(
                inId,
                inLayer
            );

            return;

        case Layer::Push::AfterLayer:
            m_renderer->pushLayerAfter(
                inId,
                inLayer
            );

            return;

        default:
            m_renderer->pushLayerBack(inLayer);

            return;
        }
    }

    void Window::run()
    {
        SDL_Event event;

        bool shouldClose = false;
        while (!shouldClose)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    shouldClose = true;

                    break;
                }

                onEvent(event);

                m_renderer->onEvent(event);
            }

            m_beginFrame = std::clock();
                m_renderer->render();
            m_endFrame   = std::clock();

            updateTelemetry();
        }
    }

    void Window::onEvent(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        case SDL_EVENT_WINDOW_SHOWN:
            refreshSize();
            refreshPosition();

            break;

        case SDL_EVENT_WINDOW_EXPOSED:
            m_isMinimized = false;

            refreshSize();

            break;

        case SDL_EVENT_WINDOW_MINIMIZED:
            m_isMinimized = true;

            break;

        case SDL_EVENT_WINDOW_RESIZED:
            refreshSize();

            break;
        
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        case SDL_EVENT_WINDOW_MOVED:
            refreshPosition();

            break;
        }

        if (!hasActiveController())
        {
            return;
        }

        getActiveController()->onEvent(inEvent);
    }

    bool Window::isFocused() const
    {
        return m_isFocused;
    }

    void Window::focus()
    {
        m_isFocused = true;

        SDL_SetWindowRelativeMouseMode(instance, SDL_TRUE);
    }

    void Window::blur()
    {
        m_isFocused = false;

        SDL_SetWindowRelativeMouseMode(instance, SDL_FALSE);
    }

    const Vec<2, int>& Window::getSize() const
    {
        if (m_isMinimized)
        {
            return VEC2_ZERO;
        }

        return m_size;
    }

    void Window::setSize(const Vec<2, int>& inSize)
    {
        setSize(inSize.x, inSize.y);
    }

    void Window::setSize(int inWidth, int inHeight)
    {
        if (!m_isResizable)
        {
            return;
        }

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

    const Vec<2, int>& Window::getDrawableSize() const
    {
        if (m_isMinimized)
        {
            return VEC2_ZERO;
        }

        return m_drawableSize;
    }

    void Window::setDrawableSize(const Vec<2, int>& inSize)
    {
        setDrawableSize(
            inSize.x,
            inSize.y
        );
    }

    void Window::setDrawableSize(int inWidth, int inHeight)
    {
        if (m_drawableSize.x == inWidth && m_drawableSize.y == inWidth)
        {
            return;
        }

        m_drawableSize.x = inWidth;
        m_drawableSize.y = inHeight;
    }

    const Vec<2, int>& Window::getPosition() const
    {
        return m_position;
    }

    void Window::setPosition(const Vec<2, int>& inPosition)
    {
        setPosition(
            inPosition.x,
            inPosition.y
        );
    }

    void Window::setPosition(int inX, int inY)
    {
        if (!m_isResizable)
        {
            return;
        }

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

    void Window::setViewport(
        const Vec<2, std::uint32_t>& inSize,
        const Vec<2, float>& inPosition
    )
    {
        if (!m_renderer)
        {
            return;
        }

        m_renderer->setViewport(inSize, inPosition);
    }

    void Window::setTitle(const std::string& inTitle)
    {
        SDL_SetWindowTitle(
            instance,
            inTitle.c_str()
        );
    }

    void Window::setIcon(const std::string& inIconPath)
    {/*
        std::string iconPath = Utils::trim(inIconPath);

        if (iconPath.empty())
        {
            return;
        }

        if (!FileSystem::exists(iconPath))
        {
            throw std::runtime_error("The file [ " + iconPath + " ] doesn't exist");
        }

        SDL_Surface* icon = STBIMG_Load(inIconPath.c_str());

        if (!icon)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_SetWindowIcon(
            instance,
            icon
        );
        SDL_FreeSurface(icon);*/
    }

    void Window::setDisplay(int inMonitorIndex)
    {
        SDL_SetWindowPosition(
            instance,
            SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex)
        );
    }

    void Window::setType(WindowType inType)
    {
        m_type = inType;

        switch (inType)
        {
        case WindowType::Windowed:
            SDL_SetWindowBordered(instance, SDL_TRUE);

            if (m_isResizable)
            {
                enableResizing();

                break;
            }
            
            disableResizing();

            break;
        
        case WindowType::WindowedBorderless:
            disableResizing();

            SDL_SetWindowBordered(instance, SDL_FALSE);

            break;
        
        case WindowType::Fullscreen:
            disableResizing();

            SDL_SetWindowFullscreen(instance, SDL_WINDOW_FULLSCREEN);

            break;
        }
    }

    WindowType Window::getType() const
    {
        return m_type;
    }

    bool Window::isResizable()
    {
        return m_isResizable;
    }

    void Window::enableResizing()
    {
        if (m_type != WindowType::Windowed)
        {
            return;
        }

        SDL_SetWindowResizable(
            instance,
            SDL_TRUE
        );

        m_isResizable = true;
    }

    void Window::disableResizing()
    {
        SDL_SetWindowResizable(
            instance,
            SDL_FALSE
        );

        m_isResizable = false;
    }

    bool Window::isMinimized()
    {
        Vec<2, int> currentSize = getSize();

        return m_isMinimized || (currentSize.x <= 0.0f || currentSize.y <= 0.0f);
    }

    void Window::initRenderer()
    {
        m_renderer = std::make_unique<Renderer>(this);
        m_rendererInternals = m_renderer->getInternals();
    }

    void Window::refreshSize()
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
    
    void Window::refreshDrawableSize()
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

    void Window::refreshPosition()
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

    void Window::initCoreLayers()
    {
        addLayer(new SkyboxLayer(this));
        addLayer(new LevelLayer( this));
        addLayer(new UILayer(    this));
    }

    void Window::updateTelemetry()
    {
        m_telemetry.delta       += m_endFrame - m_beginFrame;
        m_telemetry.frame.count += 1;

        if (m_telemetry.delta < CLOCKS_PER_SEC) {
            return;
        }

        m_telemetry.frame.rate  = std::uint32_t((m_telemetry.frame.count * 0.5) + (m_telemetry.frame.rate * 0.5));
        m_telemetry.frame.count = 0;
        m_telemetry.frame.time  = 1000.0f / float(m_telemetry.frame.rate ==0 ? 0.001 : m_telemetry.frame.rate);
        m_telemetry.delta       = 0;
    }
}