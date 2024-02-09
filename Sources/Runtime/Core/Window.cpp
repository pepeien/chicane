#include "Runtime/Core/Window.hpp"

#include "Runtime/Game.hpp"
#include "Runtime/Renderer.hpp"
#include "Runtime/Core/Layer.hpp"
#include "Runtime/Core/Layers/Level.hpp"
#include "Runtime/Core/Layers/Skybox.hpp"
#include "Runtime/Core/Layers/UI.hpp"

namespace Chicane
{
    Window::Window(
        const WindowCreateInfo& inCreateInfo,
        Controller* inController,
        Level* inLevel
    )
        : instance(nullptr),
        m_isFocused(inCreateInfo.isFocused),
        m_isResizable(inCreateInfo.isResizable),
        m_isMinimized(false),
        m_renderer(nullptr),
        m_level(inLevel),
        m_controller(inController)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
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

        initRenderer();
        initCoreLayers();
    }

    Window::~Window()
    {
        SDL_DestroyWindow(instance);
        SDL_Quit();
    }

    Stats Window::getStats()
    {
        return m_stats;
    }

    Renderer* Window::getRenderer()
    {
        return m_renderer.get();
    }

    Level* Window::getLevel()
    {
        return m_level;
    }

    void Window::addLayer(Layer* inLayer)
    {
        m_renderer->pushLayer(inLayer);
    }

    void Window::run()
    {
        SDL_Event event;

        bool shouldClose = false;
        while (shouldClose == false)
        {
            while(SDL_PollEvent(&event))
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

            m_renderer->render();

            updateStats();
        }
    }

    void Window::onEvent(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        case SDL_WINDOWEVENT:
            onWindowEvent(inEvent.window);

            break;

        case SDL_KEYDOWN:
            onKeyDown(inEvent.key);

            break;
        }
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

    Vec2 Window::getResolution()
    {
        int currentWidth  = 0;
        int currentHeight = 0;

        SDL_Vulkan_GetDrawableSize(
            instance,
            &currentWidth,
            &currentHeight
        );

        return { currentWidth, currentHeight };
    }

    void Window::setResolution(const Vec2& inResolution)
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

    Vec2 Window::getPosition()
    {
        int currentX  = 0;
        int currentY  = 0;

        SDL_GetWindowPosition(
            instance,
            &currentX,
            &currentY
        );

        return { currentX, currentY };
    }

    void Window::setTitle(const std::string& inTitle)
    {
        SDL_SetWindowTitle(
            instance,
            inTitle.c_str()
        );
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
        Vec2 currentResolution = getResolution();

        return m_isMinimized || (currentResolution.x <= 0.0f || currentResolution.y <= 0.0f);
    }

    void Window::initRenderer()
    {
        m_renderer = std::make_unique<Renderer>(this);
    }

    void Window::initCoreLayers()
    {
        addLayer(new SkyboxLayer(this));
        addLayer(new LevelLayer(this));
        addLayer(new UILayer(this));
    }

    void Window::updateStats()
    {
        m_stats.currentTime = SDL_GetTicks64() / 1000;

        uint64_t delta = m_stats.currentTime - m_stats.lastTime;

        if (delta >= 1) {
            m_stats.framerate = std::max(1, int(m_stats.count / delta));
            m_stats.lastTime  = m_stats.currentTime;
            m_stats.count     = -1;
            m_stats.time      = float(1000.0 / m_stats.framerate);
        }

        m_stats.count++;
    }

    void Window::onWindowEvent(const SDL_WindowEvent& inEvent)
    {
        switch (inEvent.event)
        {
        case SDL_WINDOWEVENT_MINIMIZED:
            m_isMinimized = true;

            break;

        case SDL_WINDOWEVENT_EXPOSED:
            m_isMinimized = false;

            break;
        }
    }

    void Window::onMouseClick()
    {
        if (!isFocused())
        {
            focus();

            return;
        }

        blur();
    }

    void Window::onKeyDown(const SDL_KeyboardEvent& inEvent)
    {
        if (m_controller == nullptr)
        {
            return;
        }

        m_controller->onEvent(inEvent.keysym.scancode);
    }
}