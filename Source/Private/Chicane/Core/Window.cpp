#include "Chicane/Core/Window.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    Window::Window(const WindowCreateInfo& inCreateInfo)
        : instance(nullptr),
        m_isFocused(inCreateInfo.isFocused),
        m_isResizable(inCreateInfo.isResizable),
        m_isMinimized(false),
        m_renderer(nullptr)
    {
        setWindow(this);

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        if (inCreateInfo.resolution.x < 0 || inCreateInfo.resolution.y < 0)
        {
            throw std::runtime_error("Invalid resolution");
        }

        instance = SDL_CreateWindow(
            inCreateInfo.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            inCreateInfo.resolution.x,
            inCreateInfo.resolution.y,
            SDL_WINDOW_VULKAN
        );

        if (instance == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }

        setDisplay(inCreateInfo.displayIndex);
        setType(inCreateInfo.type);
        setIcon(inCreateInfo.icon);

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

    Telemetry Window::getTelemetry()
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
        while (shouldClose == false)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
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
        case SDL_WINDOWEVENT:
            onWindowEvent(inEvent.window);

            break;
        }

        if (hasController() == false)
        {
            return;
        }

        getController()->onEvent(inEvent);
    }

    bool Window::isFocused()
    {
        return m_isFocused;
    }

    void Window::focus()
    {
        m_isFocused = true;

        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    void Window::blur()
    {
        m_isFocused = false;

        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    Vec<int>::Two Window::getResolution()
    {
        int currentWidth  = 0;
        int currentHeight = 0;

        SDL_Vulkan_GetDrawableSize(
            instance,
            &currentWidth,
            &currentHeight
        );

        return Vec<int>::Two(currentWidth, currentHeight);
    }

    void Window::setResolution(const Vec<int>::Two& inResolution)
    {
        if (m_isResizable)
        {
            return;
        }

        SDL_SetWindowSize(
            instance,
            inResolution.x,
            inResolution.y
        );
    }

    Vec<int>::Two Window::getPosition()
    {
        int currentX  = 0;
        int currentY  = 0;

        SDL_GetWindowPosition(
            instance,
            &currentX,
            &currentY
        );

        return Vec<int>::Two(currentX, currentY);
    }

    void Window::setTitle(const std::string& inTitle)
    {
        SDL_SetWindowTitle(
            instance,
            inTitle.c_str()
        );
    }

    void Window::setIcon(const std::string& inIconPath)
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

        SDL_Surface* icon = IMG_Load(inIconPath.c_str());

        if (!icon)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_SetWindowIcon(
            instance,
            icon
        );
        SDL_FreeSurface(icon);
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

    WindowType Window::getType()
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
        Vec<int>::Two currentResolution = getResolution();

        return m_isMinimized || (currentResolution.x <= 0.0f || currentResolution.y <= 0.0f);
    }

    void Window::initRenderer()
    {
        m_renderer = std::make_unique<Renderer>(this);
        m_rendererInternals = m_renderer->getInternals();
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
        m_telemetry.frame.time  = 1000.0f / float(m_telemetry.frame.rate ==0?0.001:m_telemetry.frame.rate);
        m_telemetry.delta       = 0;
    }

    void Window::onWindowEvent(const SDL_WindowEvent& inEvent)
    {
        switch (inEvent.event)
        {
        case SDL_WINDOWEVENT_EXPOSED:
            m_isMinimized = m_type == WindowType::Fullscreen ? true : false;

            break;

        case SDL_WINDOWEVENT_MINIMIZED:
            m_isMinimized = true;

            break;
        }
    }
}